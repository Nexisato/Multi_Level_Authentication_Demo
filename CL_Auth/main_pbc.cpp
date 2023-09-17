#include <pbc/pbc.h>
#include <pbc/pbc_test.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <string>

using chrono_time = std::chrono::_V2::system_clock::time_point;
/**
 * @brief Calculate the Time Duration between 2 points
 *
 * @param t0 : start_time
 * @param t1 : end_time
 * @return double / (unit):milliseconds
 */
double count_time(chrono_time t0, chrono_time t1) {
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    double res = double(duration.count()) *
                 std::chrono::microseconds::period::num /
                 std::chrono::milliseconds::period::den;
    return res;
}

class KGC {
private:
    element_t s;
    bool isParingInit = false;

public:
    // 椭圆曲线参数从预设的 .param 文件中读取
    bool initPairing(const char *path) {
        char param[1024];
        FILE *file = fopen(path, "r");
        size_t count = fread(param, 1, 1024, file);
        fclose(file);
        if (!count) {
            pbc_die("input error");
            return false;
        }
        pairing_init_set_buf(this->e, param, count);
        if (!pairing_is_symmetric(this->e)) {
            pbc_die("pairing must be symmetric");
            return false;
        }
        this->isParingInit = true;
        return this->isParingInit;
    }

    void SetupParams() {
        element_init_G1(this->P, this->e);
        element_init_G1(this->P0, this->e);
        element_init_Zr(this->s, this->e);
        element_random(s);
        element_random(P);
        element_mul_zn(P0, P, s);
    }

public:
    pairing_t e;
    element_t P, P0;

    void generatePartialKey(std::string &ID, element_t &partial_key) {
        element_t Qi, Di;
        element_init_G1(Qi, this->e);
        element_init_G1(Di, this->e);

        element_from_hash(Qi, const_cast<char *>(ID.c_str()), ID.length());
        element_mul_zn(Di, Qi, this->s);

        element_set(partial_key, Di);

        element_clear(Qi);
        element_clear(Di);
    }

    ~KGC() {
        pairing_clear(this->e);
        element_clear(this->P);
        element_clear(this->P0);
        element_clear(this->s);
    }
};

struct Payload {
    std::string msg;
    bool isSigned = false;
    std::pair<element_t, element_t> sigma;

    void getSignedMsg() {
        if (!this->isSigned) {
            std::cout << "[Error] payload not Signed." << std::endl;
            return;
        }
        element_printf("[Sign].U = %B\n", this->sigma.first);
        element_printf("[Sign].V = %B\n", this->sigma.second);
    }

    Payload(std::string &msg, KGC *&kgc) : msg(msg) {
        element_init_G1(this->sigma.first, kgc->e);
        element_init_GT(this->sigma.second, kgc->e);
    }

    ~Payload() {
        element_clear(this->sigma.first);
        element_clear(this->sigma.second);
    }
};

class Process {
private:
    element_t secret_key, partial_key;
    element_t secret_x; // secret value: {x}
    void generateSecretValue(KGC *&kgc) {
        element_init_Zr(this->secret_x, kgc->e);
        element_random(this->secret_x);
    }

public:
    std::string PID;
    std::pair<element_t, element_t> public_key;

    void generateKey(KGC *&kgc) {
        element_init_G1(this->partial_key, kgc->e);
        kgc->generatePartialKey(this->PID, this->partial_key);

        this->generateSecretValue(kgc);

        element_init_G1(this->secret_key, kgc->e);
        element_mul_zn(this->secret_key, this->partial_key, this->secret_x);

        element_init_G1(this->public_key.first, kgc->e);
        element_init_G1(this->public_key.second, kgc->e);

        element_mul_zn(this->public_key.first, kgc->P, this->secret_x);
        element_mul_zn(this->public_key.second, kgc->P0, this->secret_x);
    }

    void getPartialKey() {
        element_printf("Partial Key: %B\n", this->partial_key);
    }

    void getFullKey() {
        element_printf("PK_1 = %B\n", this->public_key.first);
        element_printf("PK_2 = %B\n", this->public_key.second);
        element_printf("Secret Key: %B\n", this->secret_key);
    }

    void SignMsg(Payload &payload, KGC *&kgc) {
        if (payload.isSigned) {
            std::cout << "[Error] payload already Signed." << std::endl;
            return;
        }

        element_t Msg_hash;
        element_t ri, ri_P;
        element_t T, U, V;

        element_init_Zr(ri, kgc->e);
        element_init_Zr(Msg_hash, kgc->e);
        element_init_G1(ri_P, kgc->e);
        element_init_GT(T, kgc->e);
        element_init_GT(V, kgc->e);
        element_init_G1(U, kgc->e);

        element_random(ri);
        element_mul_zn(ri_P, kgc->P, ri);
        pairing_apply(T, kgc->P, ri_P, kgc->e); // e(P, P)^r = e(P, rP)
        element_from_hash(Msg_hash, const_cast<char *>(payload.msg.c_str()),
                          payload.msg.length());
        element_mul_zn(V, T, Msg_hash); // GT <- GT * Zr
        element_mul_zn(U, this->secret_key, V);
        element_add(U, U, ri_P);

        element_set(payload.sigma.first, U);
        element_set(payload.sigma.second, V);
        payload.isSigned = true;

        // remember to clear tmp element
        element_clear(Msg_hash);
        element_clear(ri);
        element_clear(ri_P);
        element_clear(T);
        element_clear(U);
        element_clear(V);
    }

    void VerifyMsg(Payload &payload, KGC *&kgc, Process *&sender) {
        if (!payload.isSigned) {
            std::cout << "[Error] payload NOT Signed." << std::endl;
            return;
        }
        element_t lhs, rhs;
        element_init_GT(lhs, kgc->e);
        element_init_GT(rhs, kgc->e);

        pairing_apply(lhs, sender->public_key.first, kgc->P0, kgc->e);
        pairing_apply(rhs, sender->public_key.second, kgc->P, kgc->e);
        if (!element_cmp(lhs, rhs)) {
            std::cout << "[Verifiy Continue]Sender's public key is valid."
                      << std::endl;
            element_clear(lhs);
            element_clear(rhs);
        } else {
            std::cout << "[FAILED] Sender's public key is invalid."
                      << std::endl;
            element_clear(lhs);
            element_clear(rhs);
            return;
        }
        element_t PID_hash, T, msg_hash;
        element_t pair_1, pair_2;
        element_t neg_pk_second;

        element_init_G1(PID_hash, kgc->e);
        element_init_Zr(msg_hash, kgc->e);
        element_from_hash(PID_hash, const_cast<char *>(sender->PID.c_str()),
                          sender->PID.length());
        element_from_hash(msg_hash, const_cast<char *>(payload.msg.c_str()),
                          payload.msg.length());

        element_init_GT(T, kgc->e);
        element_init_GT(pair_1, kgc->e);
        element_init_GT(pair_2, kgc->e);

        element_init_G1(neg_pk_second, kgc->e);
        element_neg(neg_pk_second, sender->public_key.second);
        pairing_apply(pair_1, payload.sigma.first, kgc->P, kgc->e);
        pairing_apply(pair_2, PID_hash, neg_pk_second, kgc->e);


        element_pow_zn(pair_2, pair_2, payload.sigma.second);   // e(PID_hash, -pk_2)^V
        element_mul(T, pair_1, pair_2); // e(sigma_1, P) * e(PID_hash, -pk_2)^V

        element_t V_verify;
        element_init_GT(V_verify, kgc->e);
        element_mul_zn(V_verify, T, msg_hash);

        if (!element_cmp(V_verify, payload.sigma.second)) {
            std::cout << "[Verifiy Success] Message Verified." << std::endl;
        } else {
            std::cout << "[FAILED] Message Unverified." << std::endl;
        }  

        element_clear(T);
        element_clear(msg_hash);
        element_clear(pair_1);
        element_clear(pair_2);
        element_clear(PID_hash);
        element_clear(neg_pk_second);
        element_clear(lhs);
        element_clear(rhs);
        element_clear(V_verify);
    }

    Process(std::string id) : PID(id) {}
    ~Process() {
        element_clear(this->secret_key);
        element_clear(this->partial_key);
        element_clear(this->secret_x);
        element_clear(this->public_key.first);
        element_clear(this->public_key.second);
    }
};

int main() {
    const char *path = "../param/a.param";
    KGC *kgc = new KGC();

    // [1] Setup, system parameters generation
    std::cout << "-----[1] Setup, system parameters generation-----"
              << std::endl;
    kgc->initPairing(path);
    kgc->SetupParams();
    element_printf("P = %B\n", kgc->P);
    element_printf("P0 = %B\n", kgc->P0);

    // [2] Partial- Secret Key Generation
    std::cout << "-----[2] Partial Secret Key Generation-----" << std::endl;
    std::string PID_i = "0x000adeq01_process_i";
    std::cout << "PID_i: " << PID_i << std::endl;
    Process *process = new Process(PID_i);
    process->generateKey(kgc);
    process->getPartialKey();

    // [3] Full Key Generation for a process
    std::cout << "-----[3] Full Key Generation for a process-----" << std::endl;
    process->getFullKey();

    // [4] message SIGNED
    std::cout << "-----[4] message SIGNED process for sender -----"
              << std::endl;
    auto sign_start = std::chrono::system_clock::now();

    std::string msg_i = "hello message from process_i";
    Payload payload(msg_i, kgc);

    process->SignMsg(payload, kgc);

    payload.getSignedMsg();

    auto sign_end = std::chrono::system_clock::now();
    std::cout << "[Timing]Sign Time: " << count_time(sign_start, sign_end)
              << " ms" << std::endl;

    // [5] message Verified
    std::cout << "-----[5] message VERIFIED process for receiver -----"
              << std::endl;
    auto verify_start = std::chrono::system_clock::now();

    Process *process_j = new Process("0x000adeq01_process_j");
    process_j->generateKey(kgc);

    process_j->VerifyMsg(payload, kgc, process);



    auto verify_end = std::chrono::system_clock::now();
    std::cout << "[Timing]Verify Time: " << count_time(verify_start, verify_end)
              << " ms" << std::endl;

    return 0;
}
#include "Process_pbc.h"

#include <cstring>
#include <string>


/**
 * @brief 将 element_t 转换为字符串
 * 
 * @param element 
 * @return char* 
 */
char *element_to_string(element_t element) {
    size_t length = element_length_in_bytes(element);
    // std::cout << "(element_to_string): element_t_length: " << length
    //           << std::endl;
    uint8_t *bytes = (uint8_t *)malloc(length * sizeof(uint8_t));
    element_to_bytes(bytes, element);
    char *string = (char *)malloc((2 * length + 1) * sizeof(char));

    for (size_t i = 0; i < length; ++i) {
        sprintf(&string[2 * i], "%02X", bytes[i]);
    }

    free(bytes);
    return string;
}


/**
 * @brief 将字符串转换为 element_t
 * 
 * @param element 
 * @param string 
 */
void string_to_element(element_t element, const char *string) {
    size_t length = strlen(string) / 2;
    uint8_t *bytes = (uint8_t *)malloc(length * sizeof(uint8_t));

    for (size_t i = 0; i < length; ++i) {
        sscanf(&string[2 * i], "%02X", &bytes[i]);
    }
    element_from_bytes(element, bytes);
    free(bytes);
}


void Payload::getSignedMsg() {
    if (!this->isSigned) {
        std::cout << "[Error] payload not Signed." << std::endl;
        return;
    }
    element_printf("[Sign].U = %B\n", this->sigma.first);
    element_printf("[Sign].V = %B\n", this->sigma.second);
}

Payload::Payload(std::string &msg, KGC *&kgc) : msg(msg) {
    element_init_G1(this->sigma.first, kgc->e);
    element_init_Zr(this->sigma.second, kgc->e);
}

Payload::~Payload() {
    element_clear(this->sigma.first);
    element_clear(this->sigma.second);
}

Process::Process(std::string &id) : pid(id) {}

Process::~Process() {
    element_clear(this->secret_key);
    element_clear(this->partial_key);
    element_clear(this->secret_x);
    element_clear(this->public_key.first);
    element_clear(this->public_key.second);
}

// Private Functions:
void Process::generate_secret_value(KGC *&kgc) {
    element_init_Zr(this->secret_x, kgc->e);
    element_random(this->secret_x);
}

// Public Functions:
void Process::generate_full_key(KGC *&kgc) {
    this->generate_secret_value(kgc);

    element_init_G1(this->partial_key, kgc->e);

    kgc->generate_partial_key(this->pid, this->partial_key);

    element_init_G1(this->secret_key, kgc->e);
    element_mul_zn(this->secret_key, this->partial_key, this->secret_x);

    element_init_G1(this->public_key.first, kgc->e);
    element_init_G1(this->public_key.second, kgc->e);

    element_mul_zn(this->public_key.first, kgc->P, this->secret_x);
    element_mul_zn(this->public_key.second, kgc->P0, this->secret_x);
}

void Process::sign_msg(Payload &payload, KGC *&kgc) {
    if (payload.isSigned) {
        std::cerr << "[Error] payload already Signed." << std::endl;
        return;
    }

    element_t ri, ri_P;
    element_t T, U, V;

    element_init_Zr(ri, kgc->e);
    element_init_G1(ri_P, kgc->e);
    element_init_GT(T, kgc->e);
    element_init_Zr(V, kgc->e);
    element_init_G1(U, kgc->e);

    element_random(ri);
    element_mul_zn(ri_P, kgc->P, ri);
    pairing_apply(T, kgc->P, ri_P, kgc->e);  // e(P, P)^r = e(P, rP)

    char *T_str = element_to_string(T);
    std::cout << "T_str: " << T_str << std::endl;

    std::string msg_input = payload.msg + std::string(T_str);
    // std::cout << "msg_input: " << msg_input << std::endl;

    element_from_hash(V, const_cast<char *>(msg_input.c_str()),
                       msg_input.length());

    element_mul_zn(U, this->secret_key, V); //G1 <- G1 * Zr
    element_add(U, U, ri_P);    // G1 <- G1 + G1

    element_set(payload.sigma.first, U);
    element_set(payload.sigma.second, V);
    payload.isSigned = true;

    // remember to clear tmp element
    free(T_str);
    element_clear(ri);
    element_clear(ri_P);
    element_clear(T);
    element_clear(U);
    element_clear(V);
}

void Process::verify_msg(Payload &payload, KGC *&kgc, Process *&sender) {
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
        std::cout << "[FAILED] Sender's public key is invalid." << std::endl;
        element_clear(lhs);
        element_clear(rhs);
        return;
    }
    element_t PID_hash, T, msg_hash;
    element_t pair_1, pair_2;
    element_t neg_pk_second;

    element_init_G1(PID_hash, kgc->e);
    element_init_Zr(msg_hash, kgc->e);
    element_from_hash(PID_hash, const_cast<char *>(sender->pid.c_str()),
                      sender->pid.length());

    element_init_GT(T, kgc->e);
    element_init_GT(pair_1, kgc->e);
    element_init_GT(pair_2, kgc->e);

    element_init_G1(neg_pk_second, kgc->e);
    element_neg(neg_pk_second, sender->public_key.second);
    element_mul_zn(neg_pk_second, neg_pk_second, payload.sigma.second);// e(PID_hash, -pk_2)^V
    
    pairing_apply(pair_1, payload.sigma.first, kgc->P, kgc->e);
    pairing_apply(pair_2, PID_hash, neg_pk_second, kgc->e);

    element_mul(T, pair_1, pair_2);  // e(sigma_1, P) * e(PID_hash, -pk_2)^V

    char *T_str = element_to_string(T);
    std::string msg_input = payload.msg + std::string(T_str);
    element_from_hash(msg_hash, const_cast<char *>(msg_input.c_str()),
                       msg_input.length());

    if (!element_cmp(msg_hash, payload.sigma.second)) {
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

}

///////////// Test ////////////////

void Process::get_partial_key() {
    element_printf("[Partial Key] %B\n", this->partial_key);
}

void Process::getFullKey() {
    element_printf("[Secret Key]:  %B\n", this->secret_key);
    element_printf("[Public Key]_1:  %B\n", this->public_key.first);
    element_printf("[Public Key]_2:  %B\n", this->public_key.second);
}
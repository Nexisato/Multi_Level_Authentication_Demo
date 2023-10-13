#include "Process.h"

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
    element_init_GT(this->sigma.second, kgc->e);
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
    pairing_apply(T, kgc->P, ri_P, kgc->e);  // e(P, P)^r = e(P, rP)
    element_from_hash(Msg_hash, const_cast<char *>(payload.msg.c_str()),
                      payload.msg.length());
    element_mul_zn(V, T, Msg_hash);  // GT <- GT * Zr
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
    element_from_hash(msg_hash, const_cast<char *>(payload.msg.c_str()),
                      payload.msg.length());

    element_init_GT(T, kgc->e);
    element_init_GT(pair_1, kgc->e);
    element_init_GT(pair_2, kgc->e);

    element_init_G1(neg_pk_second, kgc->e);
    element_neg(neg_pk_second, sender->public_key.second);
    pairing_apply(pair_1, payload.sigma.first, kgc->P, kgc->e);
    pairing_apply(pair_2, PID_hash, neg_pk_second, kgc->e);

    element_pow_zn(pair_2, pair_2,
                   payload.sigma.second);  // e(PID_hash, -pk_2)^V
    element_mul(T, pair_1, pair_2);  // e(sigma_1, P) * e(PID_hash, -pk_2)^V

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
    element_clear(V_verify);
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
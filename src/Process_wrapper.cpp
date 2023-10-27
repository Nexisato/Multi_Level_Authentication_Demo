#include "Process_wrapper.h"

Process::Process(const std::string& pid_val, const KGC* kgc_ptr)
    : secret_x(Zr(kgc_ptr->e, true)),
      pid(pid_val),
      public_key(std::make_pair(G1(kgc_ptr->e), G1(kgc_ptr->e))) {
    this->partial_key = kgc_ptr->generatePartialKey(pid);

    this->secret_key = this->partial_key ^ this->secret_x;
    this->public_key.first = kgc_ptr->P ^ this->secret_x;
    this->public_key.second = kgc_ptr->P0 ^ this->secret_x;
}

Payload Process::sign(const std::string& msg, const KGC* kgc_ptr) {
    GT lhs(kgc_ptr->e), rhs(kgc_ptr->e);
    lhs = kgc_ptr->e(this->public_key.first, kgc_ptr->P0);
    rhs = kgc_ptr->e(this->public_key.second, kgc_ptr->P);
    if (lhs == rhs) {
        std::cout << "Yes" << std::endl;
    } else {
        std::cout << "No" << std::endl;
        return Payload{};
    }

    GT r(kgc_ptr->e);
    Zr a(kgc_ptr->e, true);
    G1 a_P(kgc_ptr->e);
    a_P = kgc_ptr->P ^ a;
    r = kgc_ptr->e(kgc_ptr->P, a_P);

    std::string r_str = r.toString();
    std::string tmp = msg + r_str;

    Zr v(kgc_ptr->e, const_cast<char*>(tmp.c_str()), tmp.length());

    G1 U(kgc_ptr->e);
    U = this->secret_key ^ v;
    U = U * a_P;

    Payload payload;
    payload.msg = msg;
    payload.U = U;
    payload.v = v;
    return payload;
}

bool Process::verify(Payload& payload, const KGC* kgc_ptr,
                     const Process* sender) {
    GT lhs(kgc_ptr->e), rhs(kgc_ptr->e);
    lhs = kgc_ptr->e(sender->public_key.first, kgc_ptr->P0);
    rhs = kgc_ptr->e(sender->public_key.second, kgc_ptr->P);
    if (lhs == rhs) {
        std::cout << "Yes" << std::endl;
    } else {
        std::cout << "No" << std::endl;
        return false;
    }

    GT tmp1 = kgc_ptr->e(payload.U, kgc_ptr->P);

    G1 Qi(kgc_ptr->e, const_cast<char*>(sender->pid.c_str()),
          sender->pid.length());
    G1 neg_Yi(kgc_ptr->e);
    neg_Yi = sender->public_key.second.inverse();
    neg_Yi = neg_Yi ^ payload.v;

    GT tmp2 = kgc_ptr->e(Qi, neg_Yi);

    GT r = tmp1 * tmp2;

    std::string tmp = payload.msg + r.toString();
    Zr v_tmp(kgc_ptr->e, const_cast<char*>(tmp.c_str()), tmp.length());

    if (v_tmp == payload.v) {
        std::cout << "[Verify] Success" << std::endl;
        return true;
    }
    std::cout << "[Verify] Failed" << std::endl;

    return false;
}

void Process::print_params() {
    element_printf("PK_1 = %B\n", this->public_key.first.getElement());
    element_printf("PK_2 = %B\n", this->public_key.second.getElement());
    element_printf("Secret_Key: %B\n", this->secret_key.getElement());
}
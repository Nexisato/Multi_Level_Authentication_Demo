#include "KGC_wrapper.h"


KGC::KGC(const char* param, const size_t count) : 
        e(param, count) {
    this->s = Zr(this->e, true);
    this->P = G1(this->e, false);
    this->P0 = this->P ^ this->s;
}


G1 KGC::generatePartialKey(const std::string& pid) const {
    G1 partial_key;
    G1 Qi, Di;
    Qi = G1(this->e, const_cast<char*>(pid.c_str()), pid.length());
    Di = Qi ^ this->s;
    partial_key = Di;
    return partial_key;
}


void KGC::print_params() {
    element_printf("s: %B\n", this->s.getElement());
    element_printf("P: %B\n", this->P.getElement());
    element_printf("P0: %B\n", this->P0.getElement());
}
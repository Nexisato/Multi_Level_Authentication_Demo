#include "KGC_pbc.h"

void KGC::init_pairing(const char *&path) {
    char param[1024];
    FILE *file = fopen(path, "r");
    size_t count = fread(param, 1, 1024, file);
    fclose(file);
    if (!count) {
        pbc_die("Input error");
        return;
    }
    pairing_init_set_buf(this->e, param, count);
    if (!pairing_is_symmetric(this->e)) {
        pbc_die("pairing must be symmetric");
        return;
    }

}

void KGC::setup_params() {
    element_init_G1(this->P, this->e);
    element_init_G1(this->P0, this->e);
    element_init_Zr(this->s, this->e);
    element_random(s);
    element_random(P);
    element_mul_zn(P0, P, s);
}

void KGC::generate_partial_key(std::string &pid, element_t &partial_key) {
    element_t Qi, Di;
    element_init_G1(Qi, this->e);
    element_init_G1(Di, this->e);

    element_from_hash(Qi, const_cast<char *>(pid.c_str()), pid.length());
    element_mul_zn(Di, Qi, this->s);

    element_set(partial_key, Di);

    element_clear(Qi);
    element_clear(Di);
}

KGC::KGC() {}

KGC::KGC(const char*& path) {
    this->init_pairing(path);
    this->setup_params();
}

KGC::~KGC() {
    pairing_clear(this->e);
    element_clear(this->P);
    element_clear(this->P0);
    element_clear(this->s);
}
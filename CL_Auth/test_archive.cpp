#include <string>
#include <pbc/pbc.h>
#include <pbc/pbc_test.h>
#include <iostream>
#include <time.h>
#define N 129


pairing_t* initPairing(std::string path) {
    pairing_t pairing;
    char param[1024];
    FILE* file = fopen("../param/a.param", "r");
    size_t count = fread(param, 1, 1024, file);
    fclose(file);
    if (!count) pbc_die("input error");
    pairing_init_set_buf(pairing, param, count);
    return &pairing;
}


int main(int argc, char **argv) {

    pairing_t pairing;
    char param[1024];
    FILE* file = fopen("../param/a.param", "r");
    size_t count = fread(param, 1, 1024, file);
    fclose(file);
    if (!count) pbc_die("input error");
    pairing_init_set_buf(pairing, param, count);

    element_t g, h;
    element_t public_key, secret_key;
    element_t sig;
    element_t temp1, temp2;

    element_init_G2(g, pairing);
    element_init_G2(public_key, pairing);
    element_init_G1(h, pairing);
    element_init_G1(sig, pairing);
    element_init_GT(temp1, pairing);
    element_init_GT(temp2, pairing);
    element_init_Zr(secret_key, pairing);

    element_random(g);
    element_random(secret_key);
    element_pow_zn(public_key, g, secret_key);

    element_from_hash(h, const_cast<char*>("ABCDEF"), 6);
    element_pow_zn(sig, h, secret_key);

    pairing_apply(temp1, sig, g, pairing);
    pairing_apply(temp2, h, public_key, pairing);
    if (!element_cmp(temp1, temp2)) {
        printf("signature verifies\n");
    } else {
        printf("signature does not verify\n");
    }


    pairing_clear(pairing);


    // 你像这里没有释放变量，内存会一直占用，这就造成内存泄漏了

    return 0;
}

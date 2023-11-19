#include "KGC_pariFree.h"

/// Initialize function
KGC::KGC(int nid) {
    this->ec_group = EC_GROUP_new_by_curve_name(nid);
    // const EC_POINT *P = EC_GROUP_get0_generator(ec_group);
    const BIGNUM *order = EC_GROUP_get0_order(ec_group);

    this->q = BN_dup(order);

    this->s = BN_new();
    BN_rand_range(s, q);
    this->P_pub = EC_POINT_new(ec_group);
    EC_POINT_mul(ec_group, P_pub, s, nullptr, nullptr, nullptr);
}

// Generate a partial key according to the PID value
std::vector<std::string> KGC::GeneratePartialKey(const std::string &pid) {
    BN_CTX *bn_ctx = BN_CTX_new();
    BIGNUM *r = BN_new();
    BN_rand_range(r, this->q);

    EC_POINT *R = EC_POINT_new(this->ec_group);
    // R = r * P
    EC_POINT_mul(this->ec_group, R, r, nullptr, nullptr, bn_ctx);

    std::string h_input = pid + point2hex(this->ec_group, R) +
                          point2hex(this->ec_group, this->P_pub);

    BIGNUM *h1 = string2bn(h_input);
    BIGNUM *d = BN_new();

    // d = s * h1 + r
    BN_mod_mul(d, this->s, h1, this->q, bn_ctx);
    BN_mod_add(d, d, r, this->q, bn_ctx);

    // [0][1]: Zr, point
    std::vector<std::string> res(2);
    res[0] = bn2hex(d);
    res[1] = point2hex(this->ec_group, R);

    BN_free(h1);
    BN_free(d);
    BN_CTX_free(bn_ctx);
    EC_POINT_free(R);

    return res;
}

KGC::~KGC() {
    BN_free(this->s);
    BN_free(this->q);
    EC_POINT_free(this->P_pub);
    EC_GROUP_free(this->ec_group);
}
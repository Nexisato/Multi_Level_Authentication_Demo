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

std::pair<std::string, std::string> KGC::AggregateSign(
    const std::vector<Payload> &payloads) {
    const int n = payloads.size();

    BN_CTX *bn_ctx = BN_CTX_new();

    EC_POINT *Y_total = EC_POINT_new(this->ec_group);
    BIGNUM *w_total = BN_new();

    EC_POINT *tmp_sig = EC_POINT_new(this->ec_group);
    EC_POINT *Y1_i = EC_POINT_new(this->ec_group);
    BIGNUM *u_i = BN_new();

    for (int i = 0; i < n; ++i) {
        EC_POINT_hex2point(this->ec_group, payloads[i].sig1.c_str(), Y1_i,
                           bn_ctx);
        BN_hex2bn(&u_i, const_cast<char *>(payloads[i].u_generated.c_str()));

        EC_POINT_mul(this->ec_group, tmp_sig, nullptr, Y1_i, u_i, bn_ctx);

        EC_POINT_add(this->ec_group, Y_total, Y_total, tmp_sig, bn_ctx);

        EC_POINT_clear_free(tmp_sig);
        EC_POINT_clear_free(Y1_i);
        BN_clear_free(u_i);
    }

    std::string Y = point2hex(this->ec_group, Y_total);
    std::string w = bn2hex(w_total);
    BN_free(w_total);
    EC_POINT_free(Y_total);
    BN_CTX_free(bn_ctx);
    return std::make_pair(Y, w);
}

bool KGC::AggregateVerify(const std::vector<Payload> &payloads,
                         const std::pair<std::string, std::string> &sigma,
                         const int nid, std::string &Ppub_hex,
                        std::string &acc_cur) {
    // 构造来自对方 KGC 的椭圆曲线群
    BN_CTX *bn_ctx = BN_CTX_new();
    EC_GROUP *sender_group = EC_GROUP_new_by_curve_name(nid);

    EC_POINT* lhs = EC_POINT_new(sender_group);
    EC_POINT *Y = hex2point(sender_group, sigma.first);
    BIGNUM *w = hex2bn(sigma.second);

    EC_POINT_mul(sender_group, lhs, w, nullptr, nullptr, bn_ctx);
    EC_POINT_invert(sender_group, Y, bn_ctx);
    EC_POINT_add(sender_group, lhs, lhs, Y, bn_ctx);

    EC_POINT *rhs = EC_POINT_new(sender_group);

    const int n = payloads.size();
    
    for (int i = 0; i < n; ++i) {
        std::string h1_input = payloads[i].pid + payloads[i].pk2 + Ppub_hex;
        std::string h3_input = payloads[i].msg + payloads[i].wit_new + payloads[i].pid +
                           payloads[i].pk1 + payloads[i].pk2 + payloads[i].time_stamp;
        
    }

    
    
    BN_free(w);
    EC_POINT_free(Y);
    EC_POINT_free(lhs);
    EC_GROUP_free(sender_group);
    BN_CTX_free(bn_ctx);
    return false;
}

KGC::~KGC() {
    BN_free(this->s);
    BN_free(this->q);
    EC_POINT_free(this->P_pub);
    EC_GROUP_free(this->ec_group);
}
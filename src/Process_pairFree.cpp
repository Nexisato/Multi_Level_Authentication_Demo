#include "Process_pairFree.h"

Process::Process(const std::string &pid_val, int nid,
                 const std::string &P_pub_str) {
    this->pid = pid_val;
    ;
    this->ec_group = EC_GROUP_new_by_curve_name(nid);
    const EC_POINT *P = EC_GROUP_get0_generator(ec_group);
    const BIGNUM *order = EC_GROUP_get0_order(ec_group);
    this->q = BN_dup(order);
    this->P_pub = hex2point(this->ec_group, P_pub_str);
}

bool Process::GenerateFullKey(const std::vector<std::string> &partial_key) {
    BIGNUM *d = hex2bn(partial_key[0]);
    EC_POINT *R = hex2point(this->ec_group, partial_key[1]);

    std::string h_input = this->pid + point2hex(this->ec_group, R) +
                          point2hex(this->ec_group, this->P_pub);

    BIGNUM *h1 = string2bn(h_input);
    EC_POINT *lhs = EC_POINT_new(this->ec_group);
    EC_POINT *rhs = EC_POINT_new(this->ec_group);
    // lhs = d * P
    EC_POINT_mul(this->ec_group, lhs, d, nullptr, nullptr, nullptr);
    // rhs = R + h1 * P_pub
    EC_POINT_mul(this->ec_group, rhs, nullptr, this->P_pub, h1, nullptr);
    EC_POINT_add(this->ec_group, rhs, rhs, R, nullptr);
    // Verifiy if PublicKey is correct
    if (EC_POINT_cmp(this->ec_group, lhs, rhs, nullptr) != 0) {
        return false;
    }

    BIGNUM *x = BN_new();
    BN_rand_range(x, this->q);

    EC_POINT *X = EC_POINT_new(this->ec_group);
    // X = x * P
    EC_POINT_mul(this->ec_group, X, x, nullptr, nullptr, nullptr);

    this->public_key.first = X;
    this->public_key.second = R;

    this->secret_key.first = d;
    this->secret_key.second = x;

    EC_POINT_free(lhs);
    EC_POINT_free(rhs);
    return true;
}

Payload Process::sign(const std::string &msg, std::string &wit_hex,
                      const std::string &N_hex) {
    std::string ti = getTimeTamp();  // current timestamp
    BIGNUM *ht = string2bn(ti);
    BIGNUM *N = hex2bn(N_hex);
    BIGNUM *wit = hex2bn(wit_hex);

    BN_CTX *bn_ctx = BN_CTX_new();
    // wit = wit ^ ht mod N
    BN_mod_exp(wit, wit, ht, N, bn_ctx);
    BN_free(N);

    BIGNUM *y1 = BN_new();
    BN_rand_range(y1, this->q);

    // Y1 = y1 * P
    EC_POINT *Y1 = EC_POINT_new(this->ec_group);
    EC_POINT_mul(this->ec_group, Y1, y1, nullptr, nullptr, nullptr);

    std::string h3_input = msg + bn2hex(wit) + this->pid +
                           point2hex(this->ec_group, public_key.first) +
                           point2hex(this->ec_group, public_key.second) + ti;

    std::string u_input = h3_input + point2hex(this->ec_group, Y1);

    BIGNUM *u = string2bn(u_input);
    BIGNUM *h3 = string2bn(h3_input);

    // w = (u * y1 + h3 * (x + d)) mod q
    BIGNUM *w = BN_new();

    BIGNUM *rhs = BN_new();
    BN_mod_add(rhs, this->secret_key.first, this->secret_key.second, this->q,
               bn_ctx);
    BN_mod_mul(rhs, rhs, h3, this->q, bn_ctx);

    BN_mod_mul(w, u, y1, this->q, bn_ctx);
    BN_mod_add(w, w, rhs, this->q, bn_ctx);

    Payload payload = {pid,
                       msg,
                       point2hex(this->ec_group, Y1),
                       bn2hex(w),
                       point2hex(this->ec_group, this->public_key.first),
                       point2hex(this->ec_group, this->public_key.second),
                       ti,
                       bn2hex(wit)};

    BN_free(ht);
    BN_free(wit);
    BN_free(y1);
    BN_free(u);
    BN_free(h3);
    BN_free(w);
    EC_POINT_free(Y1);
    BN_CTX_free(bn_ctx);

    return payload;
}

bool Process::verify(Payload &payload, const int nid, std::string &Ppub_hex,
                     std::string &acc_cur, std::string &N) {
    BN_CTX *bn_ctx = BN_CTX_new();
    EC_GROUP *sender_group = EC_GROUP_new_by_curve_name(nid);

    std::string ti = payload.time_stamp;  // current timestamp
    BIGNUM *ht = string2bn(ti);

    std::string h1_input = payload.pid + payload.pk2 + Ppub_hex;
    std::string h3_input = payload.msg + payload.wit_new + payload.pid +
                           payload.pk1 + payload.pk2 + ti;
    std::string u_input = h3_input + payload.sig1;

    BIGNUM *h1 = string2bn(h1_input);
    BIGNUM *h3 = string2bn(h3_input);
    BIGNUM *u = string2bn(u_input);

    EC_POINT *lhs = EC_POINT_new(sender_group);
    EC_POINT *rhs = EC_POINT_new(sender_group);

    EC_POINT *Xi = hex2point(sender_group, payload.pk1);
    EC_POINT *Ri = hex2point(sender_group, payload.pk2);
    EC_POINT *Ppub = hex2point(sender_group, Ppub_hex);

    EC_POINT *Y1 = hex2point(sender_group, payload.sig1);
    BIGNUM *wi = hex2bn(payload.sig2);

    EC_POINT_add(sender_group, rhs, Xi, Ri, bn_ctx);
    EC_POINT *rhs_tmp = EC_POINT_new(sender_group);
    EC_POINT_mul(sender_group, rhs_tmp, nullptr, Ppub, h1, bn_ctx);
    EC_POINT_add(sender_group, rhs, rhs, rhs_tmp, bn_ctx);
    EC_POINT_mul(sender_group, rhs, nullptr, rhs, h3, bn_ctx);

    EC_POINT_mul(sender_group, lhs, wi, nullptr, nullptr, bn_ctx);
    EC_POINT *lhs_tmp = EC_POINT_new(sender_group);
    // lhs_tmp = -ui * Y1
    EC_POINT_invert(sender_group, Y1, bn_ctx);
    EC_POINT_mul(sender_group, lhs_tmp, nullptr, Y1, u, bn_ctx);
    EC_POINT_add(sender_group, lhs, lhs, lhs_tmp, bn_ctx);

    if (EC_POINT_cmp(sender_group, lhs, rhs, nullptr) != 0) {
        std::cout << "[INVALID]签名不合法" << std::endl;
        return false;
    }

    std::cout << "[CONTINUE]进入累加器参数验证环节" << std::endl;
    BIGNUM *acc = hex2bn(acc_cur);
    // std::cout << "[test] 这里行不行?" << std::endl;

    std::string ti_hex = bn2hex(ht);
    std::cout << "rhs: \n";
    std::string bn_rhs = quickPow(acc_cur, ti_hex, N);
    std::cout << "\n\nlhs: \n";
    std::string bn_lhs = quickPow(payload.wit_new, payload.pid, N);

    std::cout << "bn_lhs: " << bn_lhs << std::endl;
    std::cout << "bn_rhs: " << bn_rhs << std::endl;

    if (bn_lhs != bn_rhs) {
        std::cout << "[INVALID]累加器参数不合法" << std::endl;
        return false;
    }

    std::cout << "[SUCCESS]过了" << std::endl;

    BN_CTX_free(bn_ctx);
    BN_free(ht);
    BN_free(h1);
    BN_free(h3);
    BN_free(u);
    EC_GROUP_free(sender_group);
    EC_POINT_free(lhs);
    EC_POINT_free(lhs_tmp);
    EC_POINT_free(rhs);
    EC_POINT_free(rhs_tmp);
    EC_POINT_free(Xi);
    EC_POINT_free(Ri);
    EC_POINT_free(Ppub);
    EC_POINT_free(Y1);
    BN_free(wi);
    BN_free(acc);
    return true;
}

Process::~Process() {
    BN_free(this->q);
    EC_POINT_free(this->P_pub);
    EC_GROUP_free(this->ec_group);
}
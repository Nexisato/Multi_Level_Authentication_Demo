#pragma once

#include <openssl/bn.h>
#include <openssl/obj_mac.h>

#include <vector>

#include "transfer.h"

struct Payload {
    std::string pid;
    std::string msg;
    std::string sig1, sig2;
    std::string pk1, pk2;
};

class Process {
private:
    std::pair<BIGNUM *, BIGNUM *> secret_key;

public:
    std::string pid;
    EC_GROUP *ec_group;
    BIGNUM *q;
    EC_POINT *P_pub;
    std::pair<EC_POINT *, EC_POINT *> public_key;

    Process(const std::string &pid_val, int nid, const std::string &P_pub_str) {
        this->pid = pid_val;
        ;
        this->ec_group = EC_GROUP_new_by_curve_name(nid);
        const EC_POINT *P = EC_GROUP_get0_generator(ec_group);
        const BIGNUM *order = EC_GROUP_get0_order(ec_group);
        this->q = BN_dup(order);
        this->P_pub = hex2point(this->ec_group, P_pub_str);
    }
    /**
     * @brief
     *  generate PubKey and SecKey
     PubKey(EC Point): (X, R)
        SecKey(BigNum): (d, x)
     * @param partial_key
     * @return true
     * @return false
     */
    bool GenerateFullKey(const std::vector<std::string> &partial_key) {
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
    /**
     * @brief 消息签名
     *
     * @param msg
     * @param wit_hex
     * @param acc_cur
     * @return Payload: include (pid, msg, sig1, sig2, pk1, pk2)
      sig1(EC_point): Y1
      sig2 (BIGNUM): w
     */
    Payload sign(const std::string &msg, std::string &wit_hex,
                 const std::string &acc_cur) {
        std::string ti = getTimeTamp();  // current timestamp
        BIGNUM *ht = string2bn(ti);

        BIGNUM *wit = hex2bn(wit_hex);
        BIGNUM *acc = hex2bn(acc_cur);

        BN_CTX *bn_ctx = BN_CTX_new();
        BN_mod_exp(wit, wit, ht, acc, bn_ctx);
        BN_free(acc);

        BIGNUM *y1 = BN_new();
        BN_rand_range(y1, this->q);

        // Y1 = y1 * P
        EC_POINT *Y1 = EC_POINT_new(this->ec_group);
        EC_POINT_mul(this->ec_group, Y1, y1, nullptr, nullptr, nullptr);

        std::string h3_input = msg + bn2hex(wit) + this->pid +
                               point2hex(this->ec_group, public_key.first) +
                               point2hex(this->ec_group, public_key.second) +
                               ti;

        std::string u_input = h3_input + point2hex(this->ec_group, Y1);

        BIGNUM *u = string2bn(u_input);
        BIGNUM *h3 = string2bn(h3_input);

        // w = (u * y1 + h3 * (x + d)) mod q
        BIGNUM *w = BN_new();

        BIGNUM *rhs = BN_new();
        BN_mod_add(rhs, this->secret_key.first, this->secret_key.second,
                   this->q, bn_ctx);
        BN_mod_mul(rhs, rhs, h3, this->q, bn_ctx);

        BN_mod_mul(w, u, y1, this->q, bn_ctx);
        BN_mod_add(w, w, rhs, this->q, bn_ctx);

        Payload payload = {pid,
                           msg,
                           point2hex(this->ec_group, Y1),
                           bn2hex(w),
                           point2hex(this->ec_group, this->public_key.first),
                           point2hex(this->ec_group, this->public_key.second)};

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

    /**
     * @brief 消息验签
     *
     * @param payload
     * @param nid
     * @param acc_cur
     * @return true
     * @return false
     */
    bool verify(Payload &payload, const int nid, std::string &wit,
                std::string &Ppub_hex, std::string &acc_cur) {
        BN_CTX *bn_ctx = BN_CTX_new();
        EC_GROUP *sender_group = EC_GROUP_new_by_curve_name(nid);

        std::string ti = getTimeTamp();  // current timestamp
        BIGNUM *ht = string2bn(ti);

        std::string h1_input = payload.pid + payload.pk2 + Ppub_hex;
        std::string h3_input =
            payload.msg + wit + payload.pid + payload.pk1 + payload.pk2 + ti;
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
        BIGNUM *pid_val = string2bn(payload.pid);
        BIGNUM *wit_val = hex2bn(wit);
        BIGNUM *bn_lhs = BN_new();
        BIGNUM *bn_rhs = BN_new();
        BN_exp(bn_lhs, wit_val, pid_val, bn_ctx);
        BN_exp(bn_rhs, acc, ht, bn_ctx);
        if (BN_cmp(bn_lhs, bn_rhs) != 0) {
            std::cout << "[INVALID]累加器参数验证失败" << std::endl;
            return false;
        }

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
        BN_free(pid_val);
        BN_free(wit_val);
        BN_free(bn_lhs);
        BN_free(bn_rhs);
        return true;
    }

    ~Process() {
        BN_free(this->q);
        EC_POINT_free(this->P_pub);
        EC_GROUP_free(this->ec_group);
    }
};
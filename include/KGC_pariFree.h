/**
 * @file KGC_pariFree.h
 * @author CaddiesNew
 * @brief
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 * KGC 定义，包含了生成系统参数的方法 ，以及生成部分密钥的方法
 */
#ifndef KGC_H
#define KGC_H
#pragma once
#include "transfer.h"

class KGC {
private:
    BIGNUM *s;

public:
    EC_GROUP *ec_group;
    BIGNUM *q;
    EC_POINT *P_pub;

    /**
     * @brief Construct a new KGC object
     *  initialize the system parameters according to the security number(which
     * ECC curve)
     */
    explicit KGC(int nid = NID_secp256k1);

    /**
     * @brief
     *  generate a partial key according to the PID value
     * @param pid
     * @return std::vector<std::string>
     */
    std::vector<std::string> GeneratePartialKey(const std::string &pid);

    /**
     * @brief
     *  aggregate the signature
     * @param payloads
     * @return sigma: Y(point), w(bignum)
     */
    std::pair<std::string, std::string> AggregateSign(
        const std::vector<Payload> &agg_info);

    /**
     * @brief
     *  verify the aggregated signature
     * @param aggreSigma: Y(point), w(bignum), payloads
     * @return bool: true/false
     */
    bool AggregateVerify(const std::vector<Payload> &payloads,
                         const std::pair<std::string, std::string> &sigma,
                         const int nid, std::string &Ppub_hex,
                        std::string &acc_cur);

    ~KGC();
};
#endif
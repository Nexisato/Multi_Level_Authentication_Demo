/**
 * @file Process_pairFree.h
 * @author CaddiesNew 
 * @brief 
 * @version 0.1
 * @date 2023-11-19
 * 
 * @copyright Copyright (c) 2023
 * 
 * Process 定义，包含了签名和验证的方法
 * 
 */
#ifndef PROCESS_H
#define PROCESS_H
#pragma once
#include "transfer.h"

struct Payload {
    std::string pid;
    std::string msg;
    std::string sig1, sig2;
    std::string pk1, pk2;
    std::string time_stamp;
    std::string wit_new;
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

    /**
     * @brief Construct a new Process object
     *
     *
     * @param pid_val
     * @param nid
     * @param P_pub_str
     */
    Process(const std::string &pid_val, int nid, const std::string &P_pub_str);

    /**
     * @brief
     *  generate PubKey and SecKey
        PubKey(EC Point): (X, R)
        SecKey(BigNum): (d, x)
     * @param partial_key
     * @return true
     * @return false
     */
    bool GenerateFullKey(const std::vector<std::string> &partial_key);

    /**
     * @brief 消息签名
     *
     * @param msg
     * @param wit_hex
     * @param N: public key in accumulator system
     * @return Payload: include (pid, msg, sig1, sig2, pk1, pk2)
      sig1(EC_point): Y1
      sig2 (BIGNUM): w
     */
    Payload sign(const std::string &msg, std::string &wit_hex,
                 const std::string &N_hex);

    /**
     * @brief 消息验签
     *
     * @param payload
     * @param nid
     * @param acc_cur
     * @return true
     * @return false
     */
    bool verify(Payload &payload, const int nid, std::string &Ppub_hex,
                std::string &acc_cur, std::string &N);

    ~Process();
};
#endif
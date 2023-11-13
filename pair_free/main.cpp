#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

#include <iostream>
#include <string>

#include "KGC.hpp"
#include "Process.hpp"
#include "transfer.h"

int main() {
    // // 椭圆曲线确定，生成元和阶也确定
    // const EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    // const EC_POINT *g = EC_GROUP_get0_generator(ec_group);
    // const BIGNUM *q = EC_GROUP_get0_order(ec_group);

    // 1. 初始化 kgc
    int k = 714;
    KGC *kgc_ptr = new KGC(k);

    std::string Ppub_hex = point2hex(kgc_ptr->ec_group, kgc_ptr->P_pub);

    std::cout << "Ppub_hex: " << Ppub_hex << std::endl;

    // 2. process1 初始化，kgc 生成部分私钥
    std::string pid1 = "5158ee15ec9beeae6cfcb3c5728e4313";
    Process *process1 = new Process(pid1, k, Ppub_hex);

    std::cout << "process1->P_pub: "
              << point2hex(process1->ec_group, process1->P_pub) << std::endl;
    std::vector<std::string> partial_key1 = kgc_ptr->GeneratePartialKey(pid1);

    std::cout << "partial_key1[0]: " << partial_key1[0] << std::endl;
    std::cout << "partial_key1[1]: " << partial_key1[1] << std::endl;

    // 3. 生成完整公私钥
    bool isKeyFull1 = process1->GenerateFullKey(partial_key1);
    std::string msg = "hello world";
    std::string wit1 =
        "5c3bb79df26c13832bf0ccc36d53df63e93836fbae0b82cf17486c303a002ea2bb5c26"
        "023e81723b18a65536ea24f5b8a9b1939bd60872a7007febe64eb4dd";
    std::string acc_cur =
        "535b8cc77e3535e8f631bfee85514f2b0da03a74d55d14543c0d52eabadf20a08b4f38"
        "d97390166d270828dc203896b6d8b94382aed4ed106633d74ad0b427e";

    // 4. process1 签名
    Payload payload1 = process1->sign(msg, wit1, acc_cur);
    std::cout << "payload1.sig1: " << bn2dec(hex2bn(payload1.sig1))
              << std::endl;
    std::cout << "payload1.sig2: " << bn2dec(hex2bn(payload1.sig2))
              << std::endl;

    // 5. process2 验签
    std::string pid2 = "12767b506ebefbacab00b1f080737958f";
    Process *process2 = new Process(pid2, k + 1, Ppub_hex);
    bool isVerify = process2->verify(payload1, k, wit1, Ppub_hex, acc_cur);

    return 0;
}
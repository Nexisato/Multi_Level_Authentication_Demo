#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

#include <iostream>
#include <string>

#include "KGC_pariFree.h"
#include "Process_pairFree.h"
#include "utils.h"

/**
 * @brief
 *  注意规范：除时间戳外，所有中间数值全部以16进制方式存储
 * @return int
 */
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
        "7154a3d17795d52ffb0e22f3d34c01c4134c544f3367f208f2d6c80dd6750128bfe7f6"
        "72a642a9c4a3ed644543330467e8751f3b2a80e53ca978f3a74aca0d4";
    std::string N =
        "256ab0535223b7f396e9a83baf495485643f53fe0b169abbf9c65f319a90f11fd9d6e7"
        "42bc96511164a482acad9fbd3436908ab81b80ec400738175539d10e11";
    std::string acc_cur =
        "16d09945c33747913fddf4ddb8fc0fe9548676ea060f47901d8acdc35e7476637fdcba"
        "2cb3d71fe0b09848c77ba82579b39b39a754d4d86d2031797dae92a40f";

    // 4. process1 签名
    auto start = std::chrono::system_clock::now();
    Payload payload1 = process1->sign(msg, wit1, N);
    std::cout << "payload1.sig1: " << bn2dec(hex2bn(payload1.sig1))
              << std::endl;
    std::cout << "payload1.sig2: " << bn2dec(hex2bn(payload1.sig2))
              << std::endl;

    auto end = std::chrono::system_clock::now();
    std::cout << "Sign Time: " << utils::count_time(start, end) << "ms"
              << std::endl;

    // 5. process2 验签
    auto start2 = std::chrono::system_clock::now();
    std::string pid2 = "12767b506ebefbacab00b1f080737958f";
    Process *process2 = new Process(pid2, k + 1, Ppub_hex);
    bool isVerify = process2->verify(payload1, k, Ppub_hex, acc_cur, N);

    auto end2 = std::chrono::system_clock::now();
    std::cout << "Verify Time: " << utils::count_time(start2, end2) << "ms"
              << std::endl;

    std::cout << "isVerify: " << isVerify << std::endl;


    return 0;
}
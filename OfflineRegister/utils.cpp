#include "utils.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <openssl/md5.h>
#include <random>
#include <string>

namespace utils {
mpz_class ex_gcd(mpz_class a, mpz_class b, mpz_class &x, mpz_class &y) {
    if (b == 0)
        return x = 1, y = 0, a;
    mpz_class d = ex_gcd(b, a % b, x, y);
    mpz_class t = x;
    x = y, y = t - a / b * y;
    return d;
}

mpz_class mod_reverse(mpz_class a, mpz_class mod) {
    mpz_class d, x, y;
    d = ex_gcd(a, mod, x, y);
    if (d == 1)
        return (x % mod + mod) % mod;
    else
        return -1;
}

mpz_class quick_mul(mpz_class a, mpz_class b, mpz_class mod) {
    mpz_class ans = 0;
    while (b) {
        if (b % 2 == 1)
            ans = (ans + a) % mod;
        a = (a + a) % mod;
        b /= 2;
    }
    return ans;
}

mpz_class quick_pow(mpz_class base, mpz_class power, mpz_class mod) {
    mpz_class ans = 1;
    while (power > 0) {
        if (power % 2 == 1)
            ans = quick_mul(ans, ans, mod);
        base = quick_mul(base, base, mod);
        power /= 2;
    }
    return ans % mod;
}

bool is_prime_miller_rabin(mpz_class num) {
    if (num == 2)
        return true; // 2为质数保留
    if (num % 2 != 1 || num < 2)
        return false; // 筛掉偶数和小于2的数
    mpz_class s = 0, t = num - 1;
    while (t % 2 != 1) { // 当t为偶数时，继续分解, s递增
        s++;
        t /= 2;
    }
    for (int i = 0; i < 10;
         i++) { // 二次探测定理, 进行十次探测, 使方程a^(num-1)=1(mod
                // num)成立的解有仅有a=1或者a=num-1
        gmp_randclass randz(gmp_randinit_default);
        mpz_class a = randz.get_z_range(num - 1); // 随机整数a，取(1, num-1)
        mpz_class x = quick_pow(a, t, num);       // x为二次探测的解
        for (int j = 0; j < s; j++) {             // x^s=a^(num-1)
            mpz_class test = quick_mul(x, x, num);
            if (test == 1 && x != 1 && x != num - 1)
                return false; // 若平方取模结果为1，但x不是1或者num-1，则num不是质数
            x = test;
        }
        if (x != 1)
            return false; // 费马小定理
    }
    return true;
}

mpz_class rand_big_num(int bits) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 rng(seed);

    mpz_class mpz_bits = bits;
    gmp_randclass randz(gmp_randinit_default);
    randz.seed(rng());

    return randz.get_z_bits(mpz_bits);
}

mpz_class rand_prime(int bits) {
    mpz_class prime = rand_big_num(bits);
    mpz_nextprime(prime.get_mpz_t(), prime.get_mpz_t());
    return prime;
}

mpz_class rand_safe_prime(int bits) {
    mpz_class prime = rand_prime(bits);
    mpz_class tmp = (prime - 1) / 2;
    while (!is_prime_miller_rabin(tmp)) {
        mpz_nextprime(prime.get_mpz_t(), prime.get_mpz_t());
        tmp = (prime - 1) / 2;
    }
    return prime;
}

bool is_safe_prime(mpz_class num) {
    if (!is_prime_miller_rabin(num))
        return false;
    mpz_class tmp = (num - 1) / 2;
    if (is_prime_miller_rabin(tmp))
        return true;
    return false;
}

std::string compute_file_md5(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "[Error]: could not open file" << std::endl;
        return "";
    }
    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);
    char buf[1024];
    while (file) {
        file.read(buf, sizeof(buf)); // read len[buf] bytes each time: avoid of
                                     // memory overflow
        MD5_Update(&md5_ctx, buf,
                   file.gcount()); // method depreacated in openssl 3.0, but
                                   // still works
    }
    unsigned char md5[MD5_DIGEST_LENGTH];
    MD5_Final(md5, &md5_ctx);
    char md5_str[33];
    for (int i = 0; i < 16; ++i) {
        sprintf(md5_str + i * 2, "%02x", md5[i]);
    }
    md5_str[32] = '\0';
    return std::string(md5_str);
}


std::string md5_to_entityId(const std::string &md5_str) {
    mpz_class result;
    mpz_set_str(result.get_mpz_t(), md5_str.c_str(), 16); // transfer from hex to decimal
    //std::cout << "md5_str_decimal: " << result << std::endl;
    mpz_class nonce = rand_big_num(128);
    result = result + nonce;

    mpz_nextprime(result.get_mpz_t(), result.get_mpz_t());
    while (!is_safe_prime(result)) {
        mpz_nextprime(result.get_mpz_t(), result.get_mpz_t());
    }
    return result.get_str(16);
}

void md5_to_id_test() {
    std::string md5_str = "dce0588874fd369ce493ea5bc2a21d99";
    std::string res = md5_to_entityId(md5_str);
    std::cout << "md5_str: " << md5_str << std::endl;
    std::cout << "md5_str_bitInt_safeprime: " << res << std::endl;

    mpz_class decoded_pid;
    mpz_set_str(decoded_pid.get_mpz_t(), res.c_str(), 16);
    std::cout << "decoded_pid: " << decoded_pid << std::endl;
    std::cout <<"decoded_pid_is_safeprime: " << is_safe_prime(decoded_pid) << std::endl;
}

void test() {
    mpz_class P, Q;
    int len_prime = _KEY_LEN;
    mpz_class *primerand = new mpz_class[2];
    P = rand_prime(len_prime);
    Q = rand_safe_prime(len_prime);

    std::cout << "P: " << P << std::endl;
    std::cout << "Q: " << Q << std::endl;

    std::cout << "P-1 _isPrime? : " << is_prime_miller_rabin(P - 1)
              << std::endl;
    std::cout << "P_isSafe ?: " << is_prime_miller_rabin((P - 1) / 2)
              << std::endl;
    std::cout << "Q_isSafe ?: " << is_prime_miller_rabin((Q - 1) / 2)
              << std::endl;
}
} // namespace utils
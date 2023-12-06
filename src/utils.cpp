#include "utils.h"

#include <iostream>
#include <string>

namespace utils {

double count_time(chrono_time t0, chrono_time t1) {
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    double res = double(duration.count()) *
                 std::chrono::microseconds::period::num /
                 std::chrono::milliseconds::period::den;
    return res;
}

std::vector<uint8_t> stringToBytes(const std::string& str) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(str.c_str());
    return std::vector<uint8_t>(bytes, bytes + str.size());
}

std::string bytesToString(const std::vector<uint8_t>& bytes) {
    const char* str = reinterpret_cast<const char*>(bytes.data());
    return std::string(str, bytes.size());
}

std::vector<uint8_t> xorBytes(const std::vector<uint8_t>& data1,
                              const std::vector<uint8_t>& data2) {
    std::vector<uint8_t> result;

    size_t minSize = std::min(data1.size(), data2.size());
    for (size_t i = 0; i < minSize; ++i) {
        result.push_back(data1[i] ^ data2[i]);
    }

    return result;
}

mpz_class quick_mul(mpz_class a, mpz_class b, mpz_class mod) {
    mpz_class ans;
    mpz_mul(ans.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
    mpz_mod(ans.get_mpz_t(), ans.get_mpz_t(), mod.get_mpz_t());
    return ans;
}

// mpz_class quick_pow(mpz_class base, mpz_class power, mpz_class mod) {
//     mpz_class ans;
//     mpz_powm(ans.get_mpz_t(), base.get_mpz_t(), power.get_mpz_t(),
//              mod.get_mpz_t());
//     return ans;
// }

bool is_prime_miller_rabin(mpz_class num) {
    if (num == 2) return true;                  // 2为质数保留
    if (num % 2 != 1 || num < 2) return false;  // 筛掉偶数和小于2的数
    mpz_class s = 0, t = num - 1;
    while (t % 2 != 1) {  // 当t为偶数时，继续分解, s递增
        s++;
        t /= 2;
    }
    for (int i = 0; i < 10;
         i++) {  // 二次探测定理, 进行十次探测, 使方程a^(num-1)=1(mod
                 // num)成立的解有仅有a=1或者a=num-1
        gmp_randclass randz(gmp_randinit_default);
        mpz_class a = randz.get_z_range(num - 1);  // 随机整数a，取(1, num-1)
        mpz_class x;                               // x为二次探测的解
        mpz_powm(x.get_mpz_t(), a.get_mpz_t(), t.get_mpz_t(),
                 num.get_mpz_t());     // a^t % num
        for (int j = 0; j < s; j++) {  // x^s=a^(num-1)
            mpz_class test = quick_mul(x, x, num);
            if (test == 1 && x != 1 && x != num - 1)
                return false;  // 若平方取模结果为1，但x不是1或者num-1，则num不是质数
            x = test;
        }
        if (x != 1) return false;  // 费马小定理
    }
    return true;
}

mpz_class rand_big_num(int bits) {
    // unsigned seed =
    // std::chrono::system_clock::now().time_since_epoch().count();
    // std::mt19937_64 rng(seed);

    std::random_device rd;
    std::mt19937_64 rng(rd());

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
    // mpz_class prime = rand_prime(bits);
    // mpz_class tmp = (prime - 1) / 2;

    // // return 2 if tmp must be prime, 1 if probably prime
    // while (mpz_probab_prime_p(tmp.get_mpz_t(), 15) == 0) {
    //     mpz_nextprime(prime.get_mpz_t(), prime.get_mpz_t());
    //     tmp = (prime - 1) / 2;
    // }

    // // while (!is_prime_miller_rabin(tmp)) {
    // //     mpz_nextprime(prime.get_mpz_t(), prime.get_mpz_t());
    // //     tmp = (prime - 1) / 2;
    // // }
    // return prime;
    gmp_randclass rand(gmp_randinit_default);
    rand.seed(time(NULL));

    mpz_class prime;
    while (true) {
        prime = rand.get_z_bits(bits);
        mpz_setbit(prime.get_mpz_t(), 0);  // Ensure it's odd

        // Miller-Rabin test with 15 iterations
        if (mpz_probab_prime_p(prime.get_mpz_t(), 15)) {
            mpz_class tmp = (prime - 1) / 2;

            // Check if (prime - 1) / 2 is also prime
            if (mpz_probab_prime_p(tmp.get_mpz_t(), 15)) {
                return prime;
            }
        }
    }
}

bool is_safe_prime(mpz_class num) {
    if (!is_prime_miller_rabin(num)) return false;
    mpz_class tmp = (num - 1) / 2;
    if (is_prime_miller_rabin(tmp)) return true;
    return false;
}

void test() {
    mpz_class P, Q;
    int len_prime = _KEY_LEN;
    mpz_class* primerand = new mpz_class[2];
    P = rand_prime(len_prime);
    Q = rand_safe_prime(len_prime);

    std::cout << "P: " << P.get_str() << std::endl;
    std::cout << "Q: " << Q.get_str() << std::endl;

    std::cout << "P-1 _isPrime? : " << is_prime_miller_rabin(P - 1)
              << std::endl;
    std::cout << "P_isSafe ?: " << is_safe_prime(P) << std::endl;
    std::cout << "Q_isSafe ?: " << is_safe_prime(Q) << std::endl;
}
}  // namespace utils
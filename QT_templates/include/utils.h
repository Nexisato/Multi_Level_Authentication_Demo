#pragma once

#include <gmpxx.h>

const int _KEY_LEN = 256;

namespace utils {
/**
 * @brief Extended Euclidean algorithm
 *
 * @return mpz_class : gcd(a, b)
 */
mpz_class ex_gcd(mpz_class a, mpz_class b, mpz_class &x, mpz_class &y);

/**
 * @brief a * x = 1 (mod {mod})
 * @return mpz_class : x
 */
mpz_class mod_reverse(mpz_class a, mpz_class mod);

/**
 * @brief (a * b) % mod with gmp
 *
 * @return mpz_class
 */
mpz_class quick_mul(mpz_class a, mpz_class b, mpz_class mod);
/**
 * @brief (base ^ power) % mod with gmp
 *
 * @return mpz_class
 */
mpz_class quick_pow(mpz_class base, mpz_class power, mpz_class mod);

/**
 * @brief judge whether num is prime with Miller-Rabin algorithm
 *:
 * @return true/false
 */
bool is_prime_miller_rabin(mpz_class num);

/**
 * @brief return a random big number with bits
 *
 * @param bits
 * @return mpz_class
 */
mpz_class rand_big_num(int bits);
mpz_class rand_prime(int bits);
mpz_class rand_safe_prime(int bits);
bool is_safe_prime(mpz_class num);

void test();
}  // namespace utils

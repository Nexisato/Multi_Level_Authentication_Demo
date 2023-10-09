#pragma once

#include "utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

class Accumulator {
private:
    std::pair<mpz_class, mpz_class> secret_key;
    std::vector<mpz_class> members;

    /**
     * @brief to check whether the value is in the witness list
     *
     * @return true
     * @return false
     */
    bool is_contained(const mpz_class &);

    void remove_by_pid(const mpz_class &);

public:
    std::vector<mpz_class> wits;
    mpz_class acc_init, acc_cur, public_key;
    int SYSTEM_BITS;
    explicit Accumulator(int SYSTEM_BITS);
    ~Accumulator();

    /**
     * @brief init the accumulator system parameters
     * Including: public_key, secret_key, acc_init value
     */
    void setup();

    /**
     * @brief Add a member to the accumulator by PID value
     *
     * @param pid
     */
    void add_member(const mpz_class &pid);

    /**
     * @brief Generate a witness according to the PID value
     *    (after all members are added)
     *
     * @param pid
     * @return mpz_class: witness related to the PID
     */
    mpz_class witness_generate_by_pid(mpz_class &pid);

    /**
     * @brief Generate all witnesses
     *    (after all members are added)
     *
     */
    void witness_generate_all();

    /**
     * @brief verify whether the member is in the accumulator
     *  according to the witness and PID value
     *
     * @return true/false
     */
    bool verify_member(const mpz_class &, const mpz_class &);

    /**
     * @brief Remove a member from the accumulator by PID value
     *
     * @return mpz_class : auxiliary value (to update witness)
     */
    mpz_class remove_member(const mpz_class &pid_val);

    /**
     * @brief Update all witnesses according to the auxiliary value
     * 
     */
    void update_wit_all(const mpz_class &aux);

public:
    void print_params();
    void print_wits();
};
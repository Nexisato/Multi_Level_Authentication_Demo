#include "accumulator.h"
#include <gmp.h>
#include <gmpxx.h>

Accumulator::Accumulator(int bits) {
    this->SYSTEM_BITS = bits;
    this->members.clear();
    this->wits.clear();
}

Accumulator::~Accumulator() {
    // 这两个在类外部也能访问，如果类外部访问，会出现段错误，悬挂指针
    // 容器均为 RAII（资源获取即初始化），不需要手动释放内存，清空容器即可
    this->members.clear();
    this->wits.clear();
}

// Private Functions:
bool Accumulator::is_contained(const mpz_class &pid) {
    auto it = std::find(this->members.begin(), this->members.end(), pid);
    return it != this->members.end();
}
void Accumulator::remove_by_pid(const mpz_class &pid) {
    auto it = std::find(this->members.begin(), this->members.end(), pid);
    if (it != this->members.end()) {
        size_t index = std::distance(this->members.begin(), it);
        this->members.erase(it);
        this->wits.erase(this->wits.begin() + index);
    }
}

// Public Functions:
void Accumulator::setup() {
    mpz_class p = utils::rand_safe_prime(this->SYSTEM_BITS);
    mpz_class q = utils::rand_safe_prime(this->SYSTEM_BITS);

    mpz_mul(this->public_key.get_mpz_t(), p.get_mpz_t(), q.get_mpz_t());

    this->secret_key.first = p;
    this->secret_key.second = q;

    this->acc_init = utils::rand_big_num(this->SYSTEM_BITS);
    this->acc_cur = this->acc_init;
}

void Accumulator::add_member(const mpz_class &pid) {
    this->members.emplace_back(pid);
    //this->acc_cur = utils::quick_pow(this->acc_cur, pid, this->public_key);
    mpz_powm(this->acc_cur.get_mpz_t(), this->acc_cur.get_mpz_t(),
             pid.get_mpz_t(), this->public_key.get_mpz_t());
}

mpz_class Accumulator::witness_generate_by_pid(mpz_class &pid) {
    mpz_class product = 1;
    for (const auto &pid_val : this->members) {
        if (pid_val == pid)
            continue;
        mpz_mul(product.get_mpz_t(), product.get_mpz_t(), pid_val.get_mpz_t());
    }
    //mpz_class witness = utils::quick_pow(this->acc_init, product, this->public_key);
    mpz_class witness;
    mpz_powm(witness.get_mpz_t(), this->acc_init.get_mpz_t(),
             product.get_mpz_t(), this->public_key.get_mpz_t());

    return witness;
}

void Accumulator::witness_generate_all() {
    for (auto &member : this->members) {
        mpz_class witness = witness_generate_by_pid(member);
        this->wits.emplace_back(witness);
    }
}

bool Accumulator::verify_member(const mpz_class &wit, const mpz_class &pid) {
    mpz_class rhs;
    mpz_powm(rhs.get_mpz_t(), wit.get_mpz_t(),
             pid.get_mpz_t(), this->public_key.get_mpz_t());
    return this->acc_cur == rhs;
    // return utils::quick_pow(wit, pid, this->public_key) == this->acc_cur;
}

mpz_class Accumulator::remove_member(const mpz_class &pid_val) {
    if (!this->is_contained(pid_val)) {
        std::cerr << "NOT in members according to the pid value" << std::endl;
        return 0;
    }
    mpz_class euler_pk, p_tmp, q_tmp;
    p_tmp = this->secret_key.first - 1, q_tmp = this->secret_key.second - 1;
    mpz_mul(euler_pk.get_mpz_t(), p_tmp.get_mpz_t(), q_tmp.get_mpz_t());

    mpz_class AUX = utils::mod_reverse(pid_val, euler_pk);

    //this->acc_cur = utils::quick_pow(this->acc_cur, AUX, this->public_key);  // 利用更新后的累加值验证
    mpz_powm(this->acc_cur.get_mpz_t(), this->acc_cur.get_mpz_t(), AUX.get_mpz_t(), this->public_key.get_mpz_t());

    this->remove_by_pid(pid_val);

    return AUX;
}

void Accumulator::update_wit_all(const mpz_class &update_aux) {
//#pragma omp parallel for
    for (auto &wit : this->wits) {
        //std::cout << "\noriginal_wit: " << wit << std::endl;
        //wit = utils::quick_pow(wit, update_aux, this->public_key);
        mpz_powm(wit.get_mpz_t(), wit.get_mpz_t(), update_aux.get_mpz_t(), this->public_key.get_mpz_t());
        //std::cout << "update_wit: " << wit << std::endl;
    }
}

///////////////// Public Test Functions:///////////////////////
void Accumulator::print_params() {
    std::cout << "\npublic_key: " << this->public_key << std::endl;
    std::cout << "secret_key: (" << this->secret_key.first << ", "
              << this->secret_key.second << ")" << std::endl;
    std::cout << "acc_init: " << this->acc_init << "\n" << std::endl;
}

void Accumulator::print_wits() {
    const int acc_size = this->members.size();
    for (size_t i = 0; i < acc_size; ++i) {
        std::cout << "member[" << i << "]: " << this->members[i].get_str(16) << std::endl;
        std::cout << "witness[" << i << "]: " << this->wits[i] << "\n" <<  std::endl;
    }
}
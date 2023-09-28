#include "utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

void get_pid_from_json(const char *&path, std::vector<mpz_class> &pids) {
    std::ifstream ifs(path);
    Json::Reader reader;
    Json::Value root;
    Json::Value item;
    if (!reader.parse(ifs, root, false)) {
        std::cerr << "parse error" << std::endl;
        return;
    }
    const int size = root["packages"].size();
    for (int i = 0; i < size; ++i) {
        item = root["packages"][i];
        std::string pid_str = item["pid"].asString();
        mpz_class pid_val;
        mpz_set_str(pid_val.get_mpz_t(), pid_str.c_str(), 16);
        pids.emplace_back(pid_val);
    }
}

class Accmulator {
private:
    std::pair<mpz_class, mpz_class> secret_key;
    mpz_class MODULAR = 0;
    std::vector<mpz_class> members;

    void remove_by_pid(const mpz_class &pid) {
        auto it = std::find(this->members.begin(), this->members.end(), pid);
        if (it != this->members.end()) {
            size_t index = std::distance(this->members.begin(), it);
            this->members.erase(it);
            this->wits.erase(this->wits.begin() + index);
        }
    }

public:
    std::vector<mpz_class> wits;
    mpz_class acc_init, acc, public_key;
    int bits;
    explicit Accmulator(int bits) {
        this->bits = bits;
        this->members.clear();
        this->wits.clear();
    }

    void setup() {
        mpz_class base = 2;
        mpz_pow_ui(this->MODULAR.get_mpz_t(), base.get_mpz_t(), this->bits);

        mpz_class p = utils::rand_safe_prime(this->bits);
        mpz_class q = utils::rand_safe_prime(this->bits);

        //this->public_key = utils::quick_mul(p, q, this->MODULAR);
        mpz_mul(this->public_key.get_mpz_t(), p.get_mpz_t(), q.get_mpz_t());

        this->secret_key.first = p;
        this->secret_key.second = q;
        this->acc_init = utils::rand_big_num(this->bits);
        this->acc = this->acc_init;
    }

    void print_params() {
        std::cout << "public_key: " << this->public_key << std::endl;
        std::cout << "secret_key: (" << this->secret_key.first << ", "
                  << this->secret_key.second << ")" << std::endl;
        std::cout << "acc_init: " << this->acc_init << std::endl;
    }

    void add_member(const mpz_class &pid_val) {
        this->members.emplace_back(pid_val);
        this->acc = utils::quick_pow(this->acc, pid_val, this->public_key);
        std::cout << "acc: " << this->acc << std::endl;
    }

    // generate only after all members are added
    mpz_class witness_generate(mpz_class &pid) {
        mpz_class product = 1;
//#pragma omp parallel for
        for (const auto &pid_val : this->members) {
            if (pid_val == pid)
                continue;
            mpz_mul(product.get_mpz_t(), product.get_mpz_t(),
                    pid_val.get_mpz_t());
        }
        //std::cout << "product: " << product << std::endl;   
        mpz_class witness = utils::quick_pow(this->acc_init, product, this->public_key);
        return witness;
    }


    void get_all_witness() {
        const int acc_size = this->members.size();
        // add OpenMP for parallel computing
//#pragma omp parallel for
        for (size_t i = 0; i < acc_size; ++i) {
            mpz_class witness = witness_generate(this->members[i]);
            this->wits.emplace_back(witness);
        }
    }

    void print_wits() {
        const int acc_size = this->members.size();
        for (size_t i = 0; i < acc_size; ++i) {
            std::cout << "member[" << i << "]: " << this->members[i]
                      << std::endl;
            std::cout << "witness[" << i << "]: " << this->wits[i] << std::endl;
        }
    }

    bool verify_member(const mpz_class &wit, const mpz_class &pid) {
        mpz_class ver_val = utils::quick_pow(wit, pid, this->public_key);
        std::cout << "ver_val: " << ver_val << std::endl;
        if (ver_val == this->acc)
            return true;
        return false;
    }

    mpz_class remove_member(mpz_class &pid_val) {
        mpz_class aggrX = 1;
        const int size = this->members.size();
        for (size_t i = 0; i < size; ++i) {
            aggrX = utils::quick_mul(aggrX, this->members[i], this->public_key);
        }
        // std::cout << "aggrX: " << aggrX << std::endl;

        mpz_class euler_pk =
            utils::quick_mul(this->secret_key.first - 1,
                             this->secret_key.second - 1, this->MODULAR);
        mpz_class AUX = utils::mod_reverse(aggrX, euler_pk);

        // std::cout << "eleur_pk: " << euler_pk << std::endl;
        // std::cout << "AUX: " << AUX << std::endl;

        // mpz_class tmp = utils::quick_mul(aggrX, AUX, euler_pk);
        // std::cout << "tmp: " << tmp << std::endl;   // expect 1
        this->acc = utils::quick_pow(this->acc, AUX, this->public_key);
        this->acc_init = this->acc;

        std::cout << "Before Remove: " << this->wits.size() << std::endl;
        this->remove_by_pid(pid_val);
        std::cout << "After Remove: " << this->wits.size() << std::endl;

        // TODO
        // remove pid from members, and related witness
        // don not put the function in this file
        return AUX;
    }

    void update_wit_all(mpz_class &update_aux) {
        const int size = this->wits.size();
//#pragma omp parallel for
        for (size_t i = 0; i < size; ++i) {
            this->wits[i] =
                utils::quick_pow(this->wits[i], update_aux, this->public_key);
        }
    }

    ~Accmulator() {}
};

int main() {
    const char *json_path = "../assets.json";
    std::vector<mpz_class> pids;
    get_pid_from_json(json_path, pids);

    // 1. system setup
    Accmulator *acc_ptr = new Accmulator(_KEY_LEN);
    acc_ptr->setup();
    acc_ptr->print_params();

    // 2. add members
    for (const auto &pid : pids) {
        acc_ptr->add_member(pid);
    }

    acc_ptr->get_all_witness();

    acc_ptr->print_wits();

    // 3. verify
    //#pragma omp parallel for
    for (size_t i = 0; i < pids.size(); ++i) {
        if (acc_ptr->verify_member(acc_ptr->wits[i], pids[i]))
            std::cout << "verify success" << std::endl;
        else
            std::cout << "verify failed" << std::endl;
    }

    // 4. remove member
    mpz_class pid = pids[0];
    acc_ptr->remove_member(pid);

    return 0;
}
#include "accumulator.h"
#include <json/json.h>

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

int main() {
    const char *json_path = "../assets.json";
    std::vector<mpz_class> pids;
    get_pid_from_json(json_path, pids);

    // 1. system setup
    Accumulator *acc_ptr = new Accumulator(_KEY_LEN);
    acc_ptr->setup();
    acc_ptr->print_params();

    // 2. add members
    for (const auto &pid : pids) {
        acc_ptr->add_member(pid);
    }

    acc_ptr->witness_generate_all();

    acc_ptr->print_wits();

    // 3. verify
    #pragma omp parallel for
    for (size_t i = 0; i < pids.size(); ++i) {
        if (acc_ptr->verify_member(acc_ptr->wits[i], pids[i]))
            std::cout << "verify success" << std::endl;
        else
            std::cout << "verify failed" << std::endl;
    }

    // 4. remove member
    mpz_class pid = pids[0];
    mpz_class aux = acc_ptr->remove_member(pid);

    acc_ptr->update_wit_all(aux);

    // 5. ReVerify
    #pragma omp parallel for
    for (size_t i = 0; i < acc_ptr->wits.size(); ++i) {
        if (acc_ptr->verify_member(acc_ptr->wits[i], pids[i + 1]))
            std::cout << "verify success" << std::endl;
        else
            std::cout << "verify failed" << std::endl;
    }

    return 0;
}
#include <gmpxx.h>
#include <jsoncpp/json/json.h>

#include <chrono>
#include <fstream>  // Add this line to include the <fstream> header
#include <iostream>

#include "accumulator.h"

using chrono_time = std::chrono::_V2::system_clock::time_point;
/**
 * @brief Calculate the Time Duration between 2 points
 *
 * @param t0 : start_time
 * @param t1 : end_time
 * @return double / (unit):milliseconds
 */
double count_time(chrono_time t0, chrono_time t1) {
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    double res = double(duration.count()) *
                 std::chrono::microseconds::period::num /
                 std::chrono::milliseconds::period::den;
    return res;
}

void get_pid_from_json(const char *&path, std::vector<mpz_class> &pids) {
    std::ifstream ifs(path);
    Json::Reader reader;  // Declare and initialize the 'ifs' variable
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
    auto add_start = std::chrono::system_clock::now();
    for (const auto &pid : pids) {
        acc_ptr->add_member(pid);
    }

    acc_ptr->witness_generate_all();

    acc_ptr->print_wits();

    auto add_end = std::chrono::system_clock::now();

    std::cout << "[Timing]Register Time: " << count_time(add_start, add_end)
              << " ms" << std::endl;

    // 3. verify
    const int test_time = 1;
    auto verfiy_start = std::chrono::system_clock::now();
    //#pragma omp parallel for
    for (int cnt = 0; cnt < test_time; ++cnt) {
        for (size_t i = 0; i < pids.size(); ++i) {
            if (acc_ptr->verify_member(acc_ptr->wits[i], pids[i]))
                std::cout << "verify success" << std::endl;
            else
                std::cout << "verify failed" << std::endl;
        }
    }
    auto verify_end = std::chrono::system_clock::now();
    std::cout << "[Timing]Verify Time: "
              << count_time(verfiy_start, verify_end) /
                     (test_time * pids.size())
              << " ms" << std::endl;
    std::cout << "[4]acc_cur: " << acc_ptr->acc_cur.get_str(16) << std::endl;

    // 4. remove member
    mpz_class pid = pids[0];
    std::vector<mpz_class> revoke_pids;
    revoke_pids.emplace_back(pids[0]);
    revoke_pids.emplace_back(pids[1]);

    for (const auto &revoke_pid : revoke_pids) {
        std::cout << "Remove PID: " << revoke_pid.get_str(16) << std::endl;
    }

    mpz_class aux = acc_ptr->remove_member(revoke_pids);
    acc_ptr->update_wit_all(aux);

    std::cout << "Current Wits.size(): " << acc_ptr->wits.size() << std::endl;

    std::cout << "\n\n[removed-2]acc_cur: " << acc_ptr->acc_cur.get_str(16)
              << "\n\n"
              << std::endl;

    // 5. ReVerify
    auto reverify_start = std::chrono::system_clock::now();
    //#pragma omp parallel for
    for (int cnt = 0; cnt < test_time; ++cnt) {
        for (size_t i = 0; i < acc_ptr->wits.size(); ++i) {
            if (acc_ptr->verify_member(acc_ptr->wits[i], pids[i + 2]))
                std::cout << "Reverify success" << std::endl;
            else
                std::cout << "Reverify failed" << std::endl;
        }
    }
    auto reverify_end = std::chrono::system_clock::now();
    std::cout << "[Timing]Reverify Time: "
              << count_time(reverify_start, reverify_end) /
                     (test_time * acc_ptr->wits.size())
              << " ms" << std::endl;

    return 0;
}
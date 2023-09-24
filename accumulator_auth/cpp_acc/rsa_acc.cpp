#include <iostream>
#include "utils.h"
#include <json/json.h>
#include <fstream>
#include <vector>


void get_pid_from_json(const char*& path, std::vector<mpz_class>& pids) {
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
        mpz_class pid_mpz;
        mpz_set_str(pid_mpz.get_mpz_t(), pid_str.c_str(), 16);
        pids.push_back(pid_mpz);
    }

}

class Accmulator {
private:
    mpz_class p, q;
    std::pair<mpz_class, mpz_class> secret_key;
public:
    mpz_class acc, public_key;
    Accmulator() { }

    void setup(int bits) {
        this->p = utils::rand_safe_prime(bits);
        this->q = utils::rand_safe_prime(bits);
        this->public_key = utils::quick_mul(p, q, 0);
        this->secret_key.first = p;
        this->secret_key.second = q;
        this->acc = utils::rand_big_num(bits);
    }

    ~Accmulator() {}
};


void test() {
    unsigned int bits = 50; // 设置 bits 的值
    
    // 初始化 GMP 整数对象
    mpz_class base = 2;
    mpz_class result;
    
    // 计算 2^bits
    mpz_pow_ui(result.get_mpz_t(), base.get_mpz_t(), bits);
    
    // 输出计算结果
    std::cout << "2^" << bits << " = " << result << std::endl;
    
    // 使用计算结果作为模数值
//     mpz_class modulus;
//     mpz_mod(modulus.get_mpz_t(), result.get_mpz_t(), .get_mpz_t());
    
//     // 输出模数值
//     std::cout << "Modulus: " << modulus << std::endl;
// }
}

int main() {
    const char* json_path = "../assets.json";
    std::vector<mpz_class> pids;
    get_pid_from_json(json_path, pids);

    test();




    return 0;
}
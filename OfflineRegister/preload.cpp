#include "preload.h"

namespace preload {
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
    mpz_class nonce = utils::rand_big_num(128);
    result = result + nonce;

    mpz_nextprime(result.get_mpz_t(), result.get_mpz_t());
    while (!utils::is_safe_prime(result)) {
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
    std::cout <<"decoded_pid_is_safeprime: " << utils::is_safe_prime(decoded_pid) << std::endl;
}

}; // namespace preload
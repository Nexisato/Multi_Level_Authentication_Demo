#include "utils.h"
#include <fstream>
#include <iostream>
#include <openssl/md5.h>
#include <string>

namespace preload {
/**
 * @brief compute the md5 checksum of a file
 *
 * @param path
 * @return std::string
 */
std::string compute_file_md5(const std::string &path);

/**
 * @brief covert md5 checksum to entityId (PID as proposed in the drafts)
 *  for decode: mpz_class result;
    mpz_set_str(result.get_mpz_t(), pid_str.c_str(), 16);
 * @param md5str
 * @return std::string
 */
std::string md5_to_entityId(const std::string &md5str);

void md5_to_id_test();

} // namespace preload
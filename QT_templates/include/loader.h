#ifndef LOADER_H
#define LOADER_H

#pragma once

#include <dirent.h>  //only available in linux
#include <jsoncpp/json/json.h>
#include <openssl/md5.h>

#include <fstream>
#include <iostream>
#include <string>

#include "utils.h"

namespace loader {
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

/**
 * @brief Get the File List object
 *
 * @param path  the path of the binary files directory
 * @param fileList : store the path of each binary file
 */
void get_file_list(const char *&path, std::vector<std::string> &fileList);

/**
 * @brief
 *
 * @param fileList path of each binary file
 * @param md5List  md5 checksum of each binary file
 */
void get_md5_list(std::vector<std::string> &fileList,
                  std::vector<std::string> &md5List);

/**
 * @brief write the assets to json file
 *
 * @param path
 * @param fileList
 * @param md5List
 */
void write_json(const char *&path, std::vector<std::string> &fileList,
                std::vector<std::string> &md5List);

/**
 * @brief Get the pid from json object
 *
 * @param path json file path
 * @param pids
 */
void get_pid_from_json(const char *&path, std::vector<mpz_class> &pids);

void md5_to_id_test();

bool write_json_single(const std::string &path, const std::string &filename,
                       std::string &md5, std::string &pid);
}  // namespace loader

#endif  // LOADER_H

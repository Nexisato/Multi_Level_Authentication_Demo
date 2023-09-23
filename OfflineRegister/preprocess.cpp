#include <openssl/md5.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <dirent.h> //only available in linux

void getFileList(const char *&path, std::vector<std::string>& fileList) {
    DIR *dir;
    struct dirent *ent;
    std::string base_name = std::string(path);

    if ((dir = opendir(path)) == nullptr) {
        std::cerr << "[Error]: could not open directory" << std::endl;
        return;   
    }

    while ((ent = readdir(dir)) != nullptr) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        std::string filename = base_name + std::string(ent->d_name);
        fileList.push_back(filename);    // we don't care about the file type
    
    }
    closedir(dir);
}


void compute_md5(std::vector<std::string>& fileList, std::vector<std::string>& md5List) {
    for (const auto& name : fileList) {
        std::ifstream file(name, std::ios::binary);
        if (!file) {
            std::cerr << "[Error]: could not open file" << std::endl;
            return;
        }
        MD5_CTX md5_ctx;
        MD5_Init(&md5_ctx);
        char buf[1024];
        while (file) {
            file.read(buf, sizeof(buf));    // read 1024 bytes each time: avoid of memory overflow
            MD5_Update(&md5_ctx, buf, file.gcount());
        }
        unsigned char md5[MD5_DIGEST_LENGTH];
        MD5_Final(md5, &md5_ctx);
        char md5_str[33];
        for (int i = 0; i < 16; ++i) {
            sprintf(md5_str + i * 2, "%02x", md5[i]);
        }
        md5_str[32] = '\0';
        md5List.push_back(std::string(md5_str));
    }
}


int main(int argc, char *argv[]) {
    const char *path = "../assets/";
    std::vector<std::string> fileList;
    getFileList(path, fileList);
    for (const auto& name : fileList) {
        std::cout << "File: " << name << std::endl;
    }
    std::vector<std::string> md5List;
    compute_md5(fileList, md5List);
    std::cout << "MD5 list Capacity: "  << md5List.size() << std::endl;
    for (const auto& name : md5List) {
        std::cout << "MD5: " << name << std::endl;
    }

    return 0;
}

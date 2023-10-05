#include "loader.h"

int main() {
    const char *path = "../assets/";
    std::vector<std::string> fileList;
    loader::get_file_list(path, fileList);
    for (const auto& name : fileList) {
        std::cout << "File: " << name << std::endl;
    }
    std::vector<std::string> md5List;
    loader::get_md5_list(fileList, md5List);
    std::cout << "MD5 list Capacity: "  << md5List.size() << std::endl;
    for (const auto& name : md5List) {
        std::cout << "MD5: " << name << std::endl;
    }
    const char *json_path = "../assets_test.json";
    loader::write_json(json_path, fileList, md5List);   //生成速度有点慢

    return 0;
}
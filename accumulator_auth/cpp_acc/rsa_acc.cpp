#include <iostream>
#include "flint/aprcl.h"
#include <json/json.h>
#include <fstream>


void generate_prime_from_md5(const char*& json_path) {
    Json::Reader reader;
    Json::Value root;
    std::ifstream ifs(json_path, std::ios::binary);

    if (!reader.parse(ifs, root, false)) {
        std::cout << "Failed to parse json file\n" << reader.getFormattedErrorMessages();
        return;
    }

    const int len = root["packages"].size();
    for (int i = 0; i < len; ++i) {
        Json::Value entity = root["packages"][i];
        std::string md5 = entity["md5"].asString();
        
    }

    ifs.close();
}

int main() {
    const char* json_path = "../assets.json";
    generate_prime_from_md5(json_path);


    return 0;
}
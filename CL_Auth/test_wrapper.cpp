#include "pbcwrapper/PBC.h"
#include <iostream>
#include <cstring>



int main(int argc, char **argv) {
    const char *path = "../param/a.param";
    char param[1024];
    FILE *file = fopen(path, "r");
    fclose(file);

    // 构造双线性映射
    Pairing e(param);    

    // G2 g(e);
    // Zr secret_key(e);

    // G2 public_key = g ^ secret_key;

    // G1 h(e, const_cast<char*>("ABCDEF"), 6);

    // G1 sig = h ^ secret_key;

    // //配对比较
    // GT temp1 = e(sig, g);
    // GT temp2 = e(h, public_key);

    // if (temp1 == temp2) {
    //     std::cout << "signature verifies" << std::endl;
    // } else {
    //     std::cout << "signature does not verify" << std::endl;
    // }


    // 系统参数 e, P, P0, H1, H2, 秘密值 s
    G1 P(e), P0(e);     // 默认 G1 == G2
    G1 s(e, false);
    P0 = P * s;
    std::cout << "P: " << P.getElement() << std::endl;
    std::cout << "s: " << s.getElement() << std::endl;
    std::cout << "P0: " << P0.getElement() << std::endl;

    char *PID_i = "0x0000023"; // PID_i 代表伪身份
    G1 Q_i(e, const_cast<char*>(PID_i), strlen(PID_i)); //直接从 hash 构造

    // 部分私钥
    G1 D_i = Q_i * s;





    return 0;
}
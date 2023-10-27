#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

#include "utils.h"

#include "Process_wrapper.h"

int main() {
    char param[1024];
    FILE* file = fopen("../param/a.param", "r");
    size_t count = fread(param, 1, 1024, file);
    fclose(file);
    if (!count) pbc_die("input error");

    // [1] Setup, system parameters generation
    std::cout << "\n-----[1] Setup, system parameters generation-----"
              << std::endl;
    KGC* kgc_ptr = new KGC(param, count);
    kgc_ptr->print_params();

    std::cout << "////////// [Process1] ///////////////" << std::endl;
    std::string pid1 = "197b4c349c9e1a426383f09ba576249cb";
    Process* proc1 = new Process(pid1, kgc_ptr);
    proc1->print_params();

    std::string msg = "Hello World\n";

//////////////////////////////////////////////////////////////
    auto sign_start = std::chrono::system_clock::now();
    
    Payload payload1 = proc1->sign(msg, kgc_ptr);
    
    auto sign_end = std::chrono::system_clock::now();
    std::cout << "[Timing]Sign Time: " << utils::count_time
    (sign_start, sign_end)
              << " ms" << std::endl;
//////////////////////////////////////////////////////////////
    element_printf("Signature.U: %B\n", payload1.U.getElement());
    element_printf("Signature.v: %B\n", payload1.v.getElement());

    std::cout << "///////////// [Process2] ////////////" << std::endl;
    std::string pid2 = "5158ee15ec9beeae6cfcb3c5728e4313";
    Process* proc2 = new Process(pid2, kgc_ptr);
    // proc2->print_params();

//////////////////////////////////////////////////////////////
    auto verify_start = std::chrono::system_clock::now();

    proc2->verify(payload1, kgc_ptr, const_cast<Process*>(proc1));

    auto verify_end = std::chrono::system_clock::now();
    std::cout << "[Timing]Verify Time: " << utils::count_time
    (verify_start, verify_end)
              << " ms" << std::endl;





    // std::cout << "///////////// [Msg Test] ////////////" << std::endl;
    // std::cout << "Raw Msg: " << msg << std::endl;

    // std::vector<uint8_t> msg_bytes = utils::stringToBytes(msg);
    // for (const auto& byte : msg_bytes) {
    //     std::cout << static_cast<int>(byte) << " ";
    // }
    // std::cout << std::endl;

    // std::string back_msg = utils::bytesToString(msg_bytes);
    // std::cout << "ReStore Msg: " << back_msg << std::endl;



    return 0;
}
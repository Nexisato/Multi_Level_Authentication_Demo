#include "Process.h"
#include "utils.h"


// 将 element_t 转换为字符串
std::string element_to_string(element_t element) {
    size_t length = element_length_in_bytes(element);
    std::cout << "(element_to_string): element_t_length: " << length
              << std::endl;
    uint8_t* bytes = (uint8_t*)malloc(length * sizeof(uint8_t));
    element_to_bytes(bytes, element);
    char* res = (char*)malloc((2 * length + 1) * sizeof(char));

    for (size_t i = 0; i < length; ++i) {
        sprintf(&res[2 * i], "%02X", bytes[i]);
    }

    free(bytes);

    return std::string(res);
}

// 将字符串转换为 element_t
void string_to_element(element_t element, const std::string& str) {
    char* str_tmp = const_cast<char*>(str.c_str());
    size_t length = strlen(str_tmp) / 2;
    uint8_t* bytes = (uint8_t*)malloc(length * sizeof(uint8_t));
    for (size_t i = 0; i < length; ++i) {
        sscanf(&str_tmp[2 * i], "%02X", &bytes[i]);
    }
    element_from_bytes(element, bytes);
    free(bytes);
}



int main() {
    const char *path = "../param/a.param";
    
    // [1] Setup, system parameters generation
    std::cout << "\n-----[1] Setup, system parameters generation-----"
              << std::endl;
    KGC *kgc = new KGC(path);

    element_printf("P = %B\n", kgc->P);
    element_printf("P0 = %B\n", kgc->P0);



    // [2] Partial- Secret Key Generation
    std::cout << "\n-----[2] Partial Secret Key Generation-----" << std::endl;
    std::string PID_i = "117f4a4378d49325120f76ceee7499923";
    std::cout << "PID_i: " << PID_i << std::endl;
    Process *process = new Process(PID_i);
    process->generate_full_key(kgc);
    process->get_partial_key();

    // [3] Full Key Generation for a process
    std::cout << "\n-----[3] Full Key Generation for a process-----" << std::endl;
    process->getFullKey();

    // [4] message SIGNED
    std::cout << "\n-----[4] message SIGNED process for sender -----"
              << std::endl;
    auto sign_start = std::chrono::system_clock::now();

    std::string msg_i = "hello message from process_i";
    Payload payload(msg_i, kgc);
    std::cout << "Sender PID: " << process->pid << std::endl;
    std::cout << "Message: " << msg_i << std::endl;

    process->sign_msg(payload, kgc);

    payload.getSignedMsg();

    auto sign_end = std::chrono::system_clock::now();
    std::cout << "[Timing]Sign Time: " << utils::count_time(sign_start, sign_end)
               << " ms" << std::endl;

    // [5] message Verified
    std::cout << "\n-----[5] message VERIFIED process for receiver -----"
              << std::endl;
    auto verify_start = std::chrono::system_clock::now();

    std::string pid_j = "1b434b3bcef96ae2729a244af433a33eb";
    Process *process_j = new Process(pid_j);
    process_j->generate_full_key(kgc);
    std::cout << "Receiver PID: " << process_j->pid << std::endl;
    

    process_j->verify_msg(payload, kgc, process);

    auto verify_end = std::chrono::system_clock::now();
    std::cout << "[Timing]Verify Time: " << utils::count_time(verify_start, verify_end)
              << " ms" << std::endl;




    return 0;
}
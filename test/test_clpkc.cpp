#include <chrono>
#include "Process.h"

using chrono_time = std::chrono::_V2::system_clock::time_point;
/**
 * @brief Calculate the Time Duration between 2 points
 *
 * @param t0 : start_time
 * @param t1 : end_time
 * @return double / (unit):milliseconds
 */
double count_time(chrono_time t0, chrono_time t1) {
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    double res = double(duration.count()) *
                 std::chrono::microseconds::period::num /
                 std::chrono::milliseconds::period::den;
    return res;
}


int main() {
    const char *path = "../param/a.param";
    KGC *kgc = new KGC();

    // [1] Setup, system parameters generation
    std::cout << "\n-----[1] Setup, system parameters generation-----"
              << std::endl;
    kgc->init_pairing(path);
    kgc->setup_params();
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
    std::cout << "[Timing]Sign Time: " << count_time(sign_start, sign_end)
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
    std::cout << "[Timing]Verify Time: " << count_time(verify_start, verify_end)
              << " ms" << std::endl;




    return 0;
}
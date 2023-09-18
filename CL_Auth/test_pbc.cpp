#include <pbc/pbc.h>
#include <pbc/pbc_test.h>

#include <chrono>
#include <cstring>
#include <string>
#include <iostream>


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


struct Params {
    pairing_t e;
    element_t P, P0, s;
};



int main() {
    
    pairing_t pairing;
    char param[1024];
    FILE *file = fopen("../param/a.param", "r");
    size_t count = fread(param, 1, 1024, file);
    fclose(file);
    if (!count)
        pbc_die("input error");
    pairing_init_set_buf(pairing, param, count);
    if (!pairing_is_symmetric(pairing))
        pbc_die("pairing must be symmetric");

    element_t s, P, P0;
    element_t Qi, Di;
    element_t secret_key_i, xi;
    std::pair<element_t, element_t> public_key_i;
    
    element_t Msg_hash;
    element_t ri, T, U_tmp, U, V, sigma, ri_P;
    std::pair<element_t, element_t> sigma_i;


    auto start = std::chrono::system_clock::now();
    // [1] Setup, system parameters generation
    std::cout << "-----[1] Setup, system parameters generation-----" << std::endl;
    element_init_G1(P, pairing);
    element_init_G1(P0, pairing);
    element_init_Zr(s, pairing);
    
    element_random(P);
    element_printf("P = %B\n", P);
    element_random(s);
    element_mul_zn(P0, P, s);
    element_printf("P0 = %B\n", P0);

    element_printf("master_key = %B\n", s);
    //std::cout << "C++ Stream print: " << P0 << std::endl;



    // [2] Partial- Secret Key Generation
    std::cout << "-----[2] Partial Secret Key Generation-----" << std::endl;
    std::string PID_i = "test_process_pid";
    std::cout << "PID_i: " << PID_i << std::endl;
    element_init_G1(Qi, pairing);
    element_init_G1(Di, pairing);

    element_from_hash(Qi, const_cast<char*>(PID_i.c_str()), strlen(PID_i.c_str()));
    element_printf("(Hased PID_I) Qi = %B\n", Qi); 


    element_mul_zn(Di, Qi, s);
    element_printf("(partial secret_key) Di = %B\n", Di);

    // [3] Full Key Generation for a process
    std::cout << "-----[3] Full Key Generation for a process-----" << std::endl;
    element_init_Zr (xi, pairing);
    element_init_G1(secret_key_i, pairing);
    element_init_G1 (public_key_i.first, pairing);
    element_init_G1 (public_key_i.second, pairing);

    element_random(xi);
    element_mul_zn(secret_key_i, Di, xi);
    element_printf("(full secret_key) SKi = %B\n", secret_key_i);

    element_mul_zn(public_key_i.first, P, xi);
    element_mul_zn(public_key_i.second, P0, xi);
    element_printf("(full public_key) PKi_1 = %B\n", public_key_i.first);
    element_printf("(full public_key) PKi_2 = %B\n", public_key_i.second);


    
    // [4] message SIGNED
    std::cout << "-----[4] message SIGNED process for sender -----" << std::endl;
    auto sign_start = std::chrono::system_clock::now();

    std::string msg_i = "hello message from process_i";
    std::cout << "Msg_i: " << msg_i << std::endl;

    element_init_Zr(ri, pairing);
    element_init_Zr(Msg_hash, pairing);
    element_init_G1(ri_P, pairing);

    element_init_GT(T, pairing);
    
    element_init_GT(V, pairing);
    
    element_init_G1(U_tmp, pairing);

    element_init_G1(U, pairing);


    element_random(ri);
    element_mul_zn(ri_P, P, ri);
    pairing_apply(T, P, ri_P, pairing); // e(P, P)^r = e(P, rP)

    // element_printf("(Msg SIGNED) ri = %B\n", ri);
    // element_printf("(Msg SIGNED) ri_p = %B\n", ri_P);
    element_printf("(Msg SIGNED) T = e(P, P)^r = %B\n", T);


    
    element_from_hash(Msg_hash, const_cast<char*>(msg_i.c_str()), PID_i.length());
    element_mul_zn(V, T, Msg_hash); // GT * Zr
    element_mul_zn(U_tmp, secret_key_i, V);
    element_add(U, U_tmp, ri_P);
    
    element_printf("( Hashed Msg_i) Msg_hash = %B\n", Msg_hash);

    element_init_G1(sigma_i.first, pairing); //U in G1 group
    element_init_GT(sigma_i.second, pairing);   // V in GT Group
    element_set(sigma_i.first, U);
    element_set(sigma_i.second, V);
    




    //element_printf("( Hashed Msg_i) V = T * H(msg) = %B\n", V);
    //element_printf("( Hashed Msg_i) U_tmp = V * SK_i = %B\n", U_tmp);
    //element_printf("( Hashed Msg_i) U = V * sk_i + ri*P = %B\n", U);
    element_printf("( Msg Signed) sigma_i.first = U = %B\n", sigma_i.first);
    element_printf("( Msg Signed) sigma_i.second = V = %B\n", sigma_i.second);


    auto sign_end = std::chrono::system_clock::now();
    double sign_duration = count_time(sign_start, sign_end);
    std::cout << "[Signed] Time Cost: " << sign_duration << "ms" << std::endl;



    // [5] message VERIFIED
    std::cout << "-----[5] message VERIFIED process for receiver -----" << std::endl;
    auto verify_start = std::chrono::system_clock::now();
    element_t lhs, rhs;
    element_init_GT(lhs, pairing);
    element_init_GT(rhs, pairing);

    pairing_apply(lhs, public_key_i.first, P0, pairing);
    pairing_apply(rhs, public_key_i.second, P, pairing);

    if (!element_cmp(lhs, rhs))
        printf("[CONTINUE]PublicKey Verified.\n");
    else
        printf("[FAILED] PublicKey Unverified. Try again, please.\n");

    element_t Q_receive, T_receive, msg_receive_hash;
    element_t pair_1_receive, pair_2_receive;
    element_t neg_pk_second;
    element_init_G1(Q_receive, pairing);
    element_init_Zr(msg_receive_hash, pairing);
    element_init_G1(neg_pk_second, pairing);
    element_init_GT(pair_1_receive, pairing);
    element_init_GT(pair_2_receive, pairing);
    element_init_GT(T_receive, pairing);

    element_from_hash(Q_receive, const_cast<char*>(PID_i.c_str()), PID_i.length());
    element_from_hash(msg_receive_hash, const_cast<char*>(msg_i.c_str()), PID_i.length());
    pairing_apply(pair_1_receive, sigma_i.first, P, pairing);
    // element_printf("(Signature Verify) Pair_1 = %B\n", pair_1_receive);


    element_neg(neg_pk_second, public_key_i.second);
    pairing_apply(pair_2_receive, Q_receive, neg_pk_second, pairing);

    /// V 确认是 GT 群里的元素，但是能不能作指数呢
    element_pow_zn(pair_2_receive, pair_2_receive, sigma_i.second);
    element_printf("(Signature Verify) Pair_2 = %B\n", pair_2_receive);
    ///

    element_mul(T_receive, pair_1_receive, pair_2_receive);
    //element_printf("(Signature Verify) Pair_2 = %B\n", pair_2_receive);
    element_t V_verify;
    element_init_GT(V_verify, pairing);

    element_mul_zn(V_verify, T_receive, msg_receive_hash);

    if (!element_cmp(V_verify, sigma_i.second))
        printf("[SUCCESS]Signature Verified.\n");
    else
        printf("[FAILED] Signature Unverified..\n");

    element_printf("(Signature Verify) T_receive * hash = %B\n", V_verify);

    element_printf("(Signature Verify) sigma. v = %B\n", sigma_i.second);






    auto verify_end = std::chrono::system_clock::now();
    double verify_duration = count_time(verify_start, verify_end);
    std::cout << "[Verified] Time Cost: " << verify_duration << "ms" << std::endl;
    
    



    auto end = std::chrono::system_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Total Time: "
              << double(duration.count()) *
                     std::chrono::microseconds::period::num /
                     std::chrono::milliseconds::period::den
              << "ms" << std::endl;
    printf("Have a good day!\n");

    return 0;

    return 0;
}
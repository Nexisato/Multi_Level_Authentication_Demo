#pragma once

#include <iostream>
#include <string>

#include "KGC_wrapper.h"
#include "pbcwrapper/PBC.h"

/**
 * @brief 
 * Payload sent or received by process entity
 */
struct Payload {
    std::string msg;
    G1 U;
    Zr v;
};


class Process {
private:
    Zr secret_x;
    G1 secret_key, partial_key;

public:
    std::string pid;
    std::pair<G1, G1> public_key;

    /**
     * @brief generate full public_key & secret_key for a process
     *  to communicate with other process where instantializing
     *
     * @param pid_val
     * @param kgc_ptr
     */
    explicit Process(const std::string& pid_val, const KGC* kgc_ptr);
    
    /**
     * @brief Sign the message with the secret_key
     *
     * @param msg
     * @param kgc_ptr
     */
    Payload sign(const std::string& msg, const KGC* kgc_ptr);

     /**
     * @brief Verify the message with sender's public_key
     *
     * @param payload
     * @param kgc_ptr
     * @param sender
     */
    bool verify(Payload& payload, const KGC* kgc_ptr, const Process* sender);

public:
    void print_params();
};

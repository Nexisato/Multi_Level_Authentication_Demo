#pragma once

#include <string>

#include "pbcwrapper/PBC.h"

/**
 * @brief
 * Implementation of Al'Ryami CL-PKS Algorithm
 *
 * @Class: KGC: Key Generation Center
 * @Struct Payload: Message Payload produced(recevide) by process
 * @Class: Process: Process Entity in the system
 */
class KGC {
private:
    Zr s;
public:
    G1 P, P0;
    Pairing e;

    /**
     * @brief Initalize the Bilinear Pairing Registration in PBC
     *  this->e: bilinear pairing
     *
     * @param path  .param file path
     * @param count fread return value
     * @return true/false
     */
    explicit KGC(const char* param, const size_t count);
    /**
     * @brief Generate the Partial Key for Process
     *
     * @param pid
     * @param partial_key
     */
    G1 generatePartialKey(const std::string& pid) const;

    ~KGC(){}
public:
    void print_params();
};
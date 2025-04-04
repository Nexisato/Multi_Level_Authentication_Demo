#pragma once

#include <string>

#include "pbc/pbc.h"

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
    element_t s;

public:
    /**
     * @brief Initalize the Bilinear Pairing Registration in PBC
     *  this->e: bilinear pairing
     *
     * @param path  .param file path
     * @return true/false
     */
    void init_pairing(const char *&path);

    /**
     * @brief Initialize the System Parameters
     *   Including: P, P0, s(master secret key)
     */
    void setup_params();

public:
    pairing_t e;
    element_t P, P0;

    /**
     * @brief Generate the Partial Key for Process
     *
     * @param pid
     * @param partial_key
     */
    void generate_partial_key(std::string &pid, element_t &partial_key);

    KGC();
    KGC(const char *&path);
    ~KGC();
};
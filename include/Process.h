#pragma once

#include "pbc/pbc.h"
#include <iostream>
#include <string>
#include "KGC.h"

struct Payload {
    std::string msg;
    bool isSigned = false;
    std::pair<element_t, element_t> sigma;

    Payload(std::string &, KGC*&);
    ~Payload();
    void getSignedMsg();
};


class Process {
private:
    element_t secret_key, partial_key;
    element_t secret_x; // secret value: {x}

    /**
     * @brief generate the secret value {x} for a process
     *
     * @param kgc
     */
    void generate_secret_value(KGC*&);

public:
    std::string pid;
    std::pair<element_t, element_t> public_key;

    Process(std::string &id);
    ~Process();

    /**
     * @brief generate the public_key & secret_key for a process
     *  to communicate with other process
     *
     * @param KGC
     */
    void generate_full_key(KGC*&);

    /**
     * @brief Sign the message with the secret_key
     *
     * @param payload
     * @param kgc
     */
    void sign_msg(Payload &payload, KGC *&kgc);

    /**
     * @brief Verify the message with sender's public_key
     *
     * @param payload
     * @param kgc
     * @param sender
     */
    void verify_msg(Payload &payload, KGC *&kgc, Process *&sender);

public:
    void get_partial_key();
    void getFullKey();
};

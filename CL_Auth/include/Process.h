#include <pbc/pbc.h>
#include <pbc/pbc_test.h>

#include <cstring>
#include <iostream>
#include <string>

class KGC;

struct Payload {
    std::string msg;
    bool isSigned = false;
    std::pair<element_t, element_t> sigma;

    void getSignedMsg() {
        if (!this->isSigned) {
            std::cout << "[Error] payload not Signed." << std::endl;
            return;
        }
        element_printf("[Sign].U = %B\n", this->sigma.first);
        element_printf("[Sign].V = %B\n", this->sigma.second);
    }

    Payload(std::string &msg, KGC *&kgc) : msg(msg) {
        element_init_G1(this->sigma.first, kgc->e);
        element_init_GT(this->sigma.second, kgc->e);
    }

    ~Payload() {
        element_clear(this->sigma.first);
        element_clear(this->sigma.second);
    }
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
    void generate_secret_value(KGC *&kgc);

public:
    std::string pid;
    std::pair<element_t, element_t> public_key;

    Process(std::string &id);
    ~Process();

    /**
     * @brief generate the public_key & secret_key for a process
     *  to communicate with other process
     *
     * @param kgc
     */
    void generate_full_key(KGC *&kgc);

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

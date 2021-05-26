#include <string>

#ifndef VIGENERECIPHER_H
#define VIGENERECIPHER_H

class vigenereCipher {
public:
    static std::string formatKey(const std::string &ciphertext, const std::string &key);
    static std::string decrypt(std::string ciphertext, std::string key);
};

#endif //VIGENERECIPHER_H
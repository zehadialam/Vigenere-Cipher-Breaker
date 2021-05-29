#include <string>

#ifndef VIGENERECIPHER_H
#define VIGENERECIPHER_H

class vigenereCipher {
public:
    static std::string formatKey(const std::string &ciphertext, const std::string &key);

    static std::string decrypt(std::string ciphertext, std::string key);

    static void setKeyLength(int length);

    static int getKeyLength();

private:
    inline static int keyLength;
};

#endif // VIGENERECIPHER_H
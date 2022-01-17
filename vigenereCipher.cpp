#include "vigenereCipher.h"

std::string vigenereCipher::formatKey(const std::string &ciphertext, const std::string &key) {
    std::string cipherTextLengthKey{};
    for (int i = 0; i < (int) ciphertext.length(); i += (int) key.length()) {
        cipherTextLengthKey += key;
    }
    return cipherTextLengthKey.substr(0, ciphertext.length());
}

std::string vigenereCipher::decrypt(std::string ciphertext, std::string key) {
    std::string decrypted{};
    for (int i = 0; i < (int) ciphertext.length(); i++) {
        decrypted += (char) toupper((((tolower(ciphertext[i] - 97)) - (tolower(key[i] - 97))) % 26 + 26) % 26 + 97);
    }
    return decrypted;
}

void vigenereCipher::setKeyLength(int length) {
    vigenereCipher::keyLength = length;
}

int vigenereCipher::getKeyLength() {
    return vigenereCipher::keyLength;
}



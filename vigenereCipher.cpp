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
    int ciphertextLength = ciphertext.length();
    for (int i = 0; i < ciphertextLength; i++) {
        // Get the current ciphertext and key characters
        char c = ciphertext[i];
        char k = key[i];
        int decryptedChar = ((c - 65) - (k - 65)) % 26; // Decrypt the ciphertext character
        if (decryptedChar < 0) { // handle negative values
            decryptedChar += 26;
        }
        // Convert the decrypted character back to uppercase and add it to the decrypted message
        char decryptedCharUpper = decryptedChar + 65;
        decrypted += decryptedCharUpper;
    }
    return decrypted;
}

void vigenereCipher::setKeyLength(int length) {
    vigenereCipher::keyLength = length;
}

int vigenereCipher::getKeyLength() {
    return vigenereCipher::keyLength;
}



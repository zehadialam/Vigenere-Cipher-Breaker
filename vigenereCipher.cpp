/**
 * @author Zehadi Alam
 */
 
#include "vigenereCipher.h"

/**
 * Formats the key string so that it is at least as long as the ciphertext string.
 *
 * @param ciphertext The ciphertext string.
 * @param key The key string.
 * @return The formatted key string.
 */
std::string vigenereCipher::formatKey(const std::string &ciphertext, const std::string &key) {
    std::string cipherTextLengthKey{};
    int ciphertextLength = ciphertext.length();
    int keyLength = key.length();
    // Repeatedly append the key to the cipherTextLengthKey string until it is at least 
    // as long as the ciphertext
    for (int i = 0; i < ciphertextLength; i += keyLength) {
        cipherTextLengthKey += key;
    }
    // Return a substring of the formatted key that is the same length as the ciphertext
    return cipherTextLengthKey.substr(0, ciphertext.length());
}

/**
 * Decrypts a ciphertext string using the Vigenère cipher with the given key.
 *
 * @param ciphertext The ciphertext string to decrypt.
 * @param key The key to use for decryption.
 * @return The decrypted plaintext string.
 */
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

/**
 * Sets the length of the key used for encryption and decryption.
 *
 * @param length The length of the key.
 */
void vigenereCipher::setKeyLength(int length) {
    vigenereCipher::keyLength = length;
}

/**
 * Gets the length of the key used for encryption and decryption.
 *
 * @return The length of the key.
 */
int vigenereCipher::getKeyLength() {
    return vigenereCipher::keyLength;
}

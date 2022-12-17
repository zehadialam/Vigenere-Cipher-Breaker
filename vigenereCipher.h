/**
 * @file vigenere_cipher.h
 *
 * @brief Header file for the Vigenère cipher.
 *
 * @author Zehadi Alam
 */

#ifndef VIGENERECIPHER_H
#define VIGENERECIPHER_H

#include <string>

/**
 * @class vigenereCipher
 *
 * @brief Utility class for providing tools to use in the Vigenère cipher breaker program.
 *
 * This class provides static methods for formatting the key string, decrypting ciphertext strings,
 * setting the key length, and getting the key length.
 */
class vigenereCipher {
public:
   /**
    * Formats the key string so that it is at least as long as the ciphertext string.
    *
    * @param ciphertext The ciphertext string.
    * @param key The key string.
    * @return The formatted key string.
    */
    static std::string formatKey(const std::string &ciphertext, const std::string &key);

   /**
    * Decrypts a ciphertext string using the Vigenère cipher with the given key.
    *
    * @param ciphertext The ciphertext string to decrypt.
    * @param key The key to use for decryption.
    * @return The decrypted plaintext string.
    */
    static std::string decrypt(std::string ciphertext, std::string key);

   /**
    * Sets the length of the key used for encryption and decryption.
    *
    * @param length The length of the key.
    */
    static void setKeyLength(int length);

   /**
    * Gets the length of the key used for encryption and decryption.
    *
    * @return The length of the key.
    */
    static int getKeyLength();

private:
   /**
    * The length of the key used for encryption and decryption.
    */
    inline static int keyLength;
};

#endif // VIGENERECIPHER_H
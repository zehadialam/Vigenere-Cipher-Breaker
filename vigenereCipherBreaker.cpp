#include <chrono>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <thread>
#include <valarray>
#include "nGramScorer.h"
#include "vigenereCipher.h"

using std::string;
using std::cout;

/**
 * Return the English alphabet as a string
 * @return the English alphabet
 */
string theAlphabet() {
    return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

/**
 * Return all-caps ciphertext with spaces and punctuation removed.
 * @param ciphertext the specified ciphertext
 * @return all-caps ciphertext with spaces and punctuation removed.
 */
string formatCiphertext(const string &ciphertext) {
    string formattedCiphertext{};
    for (char c : ciphertext) {
        if (isalpha(c)) {
            formattedCiphertext += (char) toupper(c);
        }
    }
    return formattedCiphertext;
}

/**
 * Return the original format of the ciphertext, including letter casing, spaces, punctuation,
 * and non-alphabetic characters
 * @param originalFormat original ciphertext format
 * @param modifiedFormat ciphertext format where the letter casing, spaces, punctuation,
 * and non-alphabetic characters have been modified
 * @return the original format of the ciphertext, including letter casing, spaces, punctuation,
 * and non-alphabetic characters
 */
string restoreOriginalFormat(const string &originalFormat, string modifiedFormat) {
    string restoredText{};
    int j = 0;
    for (char i : originalFormat) {
        if (isalpha(i)) {
            if (isupper(i)) {
                restoredText += (char) toupper(modifiedFormat[j]);
            } else if (islower(i)) {
                restoredText += (char) tolower(modifiedFormat[j]);
            }
            j++;
        } else {
            restoredText += i;
        }
    }
    return restoredText;
}

/**
 * Return a permutation of an ngram
 * @param n the number of characters making up the ngram
 * @param permutationCount the number designating a particular permutation
 * @param alphabet the english alphabet
 * @return a permutation of an ngram
 */
string ngramPermutation(int n, int permutationCount, string alphabet) {
    string ngram{};
    for (int i = 0; i < n; i++) {
        ngram += alphabet[permutationCount % 26];
        permutationCount /= 26;
    }
    return ngram;
}

/**
 * Return the first n letters of the potential decryption key
 * @param ngram a particular nGramScorer object
 * @param n the number of characters making up the ngram
 * @param alphabet the English alphabet
 * @param ciphertext the ciphertext to be decrypted
 * @param keyLength the number of characters of the key
 * @return the first n letters of the potential decryption key
 */
string firstNKeyLetters(nGramScorer ngram, int n, const string &alphabet, const string &ciphertext, int keyLength) {
    std::map<double, string> keyCandidates;
    string pad(keyLength - n, 'A');
    for (int i = 0; i < (int) pow(26, n); i++) {
        string key = ngramPermutation(n, i, alphabet) + pad;
        string formattedKey = vigenereCipher::formatKey(ciphertext, key);
        string plaintext = vigenereCipher::decrypt(ciphertext, formattedKey);
        double score = 0;
        for (int j = 0; j < (int) ciphertext.length(); j += keyLength) {
            if (j + n < (int) plaintext.length()) {
                score += ngram.score(plaintext.substr(j, n));
            }
        }
        keyCandidates[score] = key.substr(0, n);
    }
    return keyCandidates[keyCandidates.rbegin()->first];
}

/**
 * Return the full potential decryption key
 * @param ngram a particular nGramScorer object
 * @param n the number of characters making up the ngram
 * @param alphabet the English alphabet
 * @param ciphertext the ciphertext to be decrypted
 * @param keyLength the number of characters of the key
 * @param keyBuilder the result of the firstNKeyLetters() function
 * @return the full potential decryption key
 */
string
fullKeyNormal(nGramScorer ngram, int n, const string &alphabet, const string &ciphertext, int keyLength,
              string keyBuilder) {
    std::map<double, string> keyCandidates;
    for (int i = 0; i < keyLength - n; i++) {
        keyCandidates.clear();
        for (char c : alphabet) {
            string partialKey = keyBuilder + c;
            string pad(keyLength - partialKey.length(), 'A');
            string fullKey = partialKey + pad;
            string fullKeyFormatted = vigenereCipher::formatKey(ciphertext, fullKey);
            string plaintext = vigenereCipher::decrypt(ciphertext, fullKeyFormatted);
            double score = 0;
            for (int j = 0; j < (int) ciphertext.length(); j += keyLength) {
                if (j + partialKey.length() < plaintext.length()) {
                    score += ngram.score(plaintext.substr(j, partialKey.length()));
                }
            }
            keyCandidates[score] = partialKey;
        }
        //cout << keyCandidates[keyCandidates.rbegin()->first] << endl;
        keyBuilder = keyCandidates[keyCandidates.rbegin()->first];
    }
    return keyCandidates[keyCandidates.rbegin()->first];
}

/**
 * Return the full potential decryption key in a stronger deciphering attempt
 * @param ngram a particular nGramScorer object
 * @param n the number of characters making up the ngram
 * @param alphabet the English alphabet
 * @param ciphertext the ciphertext to be decrypted
 * @param keyLength the number of characters of the key
 * @param keyBuilder the result of the firstNKeyLetters() function
 * @return the full potential decryption key in a stronger deciphering attempt
 */
string
fullKeyStronger(nGramScorer ngram, int n, const string &alphabet, const string &ciphertext, int keyLength,
                string keyBuilder) {
    std::map<double, string> keyCandidates;
    for (int i = 0; i < (int) (keyLength / 3) - n; i++) {
        keyCandidates.clear();
        for (int j = 0; j < (int) pow(26, 3); j++) {
            string partialKey = keyBuilder + ngramPermutation(3, j, alphabet);
            string pad(keyLength - partialKey.length(), 'A');
            string fullKey = partialKey + pad;
            string fullKeyFormatted = vigenereCipher::formatKey(ciphertext, fullKey);
            string plaintext = vigenereCipher::decrypt(ciphertext, fullKeyFormatted);
            double score = 0;
            for (int k = 0; k < (int) ciphertext.length(); k += keyLength) {
                if (k + partialKey.length() < plaintext.length()) {
                    score += ngram.score(plaintext.substr(k, partialKey.length()));
                }
            }
            keyCandidates[score] = partialKey;
        }
        // cout << keyCandidates[keyCandidates.rbegin()->first] << endl;
        keyBuilder = keyCandidates[keyCandidates.rbegin()->first];
    }
    for (int i = (int) keyBuilder.length(); i < keyLength; i++) {
        keyCandidates.clear();
        for (char c : alphabet) {
            string partialKey = keyBuilder + c;
            string pad(keyLength - partialKey.length(), 'A');
            string fullKey = partialKey + pad;
            string fullKeyFormatted = vigenereCipher::formatKey(ciphertext, fullKey);
            string plaintext = vigenereCipher::decrypt(ciphertext, fullKeyFormatted);
            double score = 0;
            for (int j = 0; j < (int) ciphertext.length(); j += keyLength) {
                if (j + partialKey.length() < plaintext.length()) {
                    score += ngram.score(plaintext.substr(j, partialKey.length()));
                }
            }
            keyCandidates[score] = partialKey;
        }
        // cout << keyCandidates[keyCandidates.rbegin()->first] << endl;
        keyBuilder = keyCandidates[keyCandidates.rbegin()->first];
    }
    return keyCandidates[keyCandidates.rbegin()->first];
}

/**
 * Prints the verbose results of an attempt to break the decryption key
 * @param bestScore the best ngram score of a string
 * @param tryKeyLength the length of a potential decryption key
 * @param tryKey a potential decryption key
 * @param originalCipherText the ciphertext with spaces, punctuation, and non-alphabetic
 * characters
 * @param formattedCipherText all-caps ciphertext with spaces and punctuation removed
 */
void printVerboseResults(double bestScore, int tryKeyLength, const string &tryKey, const string &originalCipherText,
                         const string &formattedCipherText) {
    cout << "Score: " << std::setprecision(16) << bestScore << ", " << "Key length: " << tryKeyLength << ", "
         << "Key: " << tryKey << "\n"
         << "Decrypted: "
         << restoreOriginalFormat(originalCipherText, vigenereCipher::decrypt(formattedCipherText,
                                                                              vigenereCipher::formatKey(
                                                                                      formattedCipherText,
                                                                                      tryKey)))
         << "\n\n";
}

/**
 * Prints the non-verbose results of an attempt to break the decryption key
 * @param keyLength the length of the potential decryption key
 * @param key a potential decryption key
 * @param originalCipherText the ciphertext with spaces, punctuation, and non-alphabetic
 * characters
 * @param formattedCipherText all-caps ciphertext with spaces and punctuation removed
 */
void
printResults(int keyLength, const string &key, const string &originalCipherText, const string &formattedCipherText) {
    cout << "POTENTIAL MATCH FOUND\n" << "\n";
    cout << "KEY LENGTH: " << keyLength << "\n";
    cout << "KEY: " << key << "\n" << "\n";
    cout << "DECRYPTED MESSAGE:" << "\n";
    cout << restoreOriginalFormat(originalCipherText, vigenereCipher::decrypt(formattedCipherText,
                                                                              vigenereCipher::formatKey(
                                                                                      formattedCipherText, key)))
         << "\n\n";
}

/**
 * Constructs a key based on ngram scoring to potentially decrypt the message
 * @param n1 an nGramScorer object
 * @param n2 an nGramScorer object
 * @param n the number of characters making up the ngram
 * @param rangeStart the minimum key length to try
 * @param rangeEnd the maximum key length to try
 * @param alphabet the English alphabet
 * @param originalCipherText the ciphertext with spaces, punctuation, and non-alphabetic
 * characters
 * @param formattedCipherText all-caps ciphertext with spaces and punctuation removed
 * @param verboseMode specify whether or not to use verbose mode
 */
void
breakEncryption(const nGramScorer &n1, nGramScorer n2, int n, int rangeStart, int rangeEnd, const string &alphabet,
                const string &originalCipherText, const string &formattedCipherText, bool verboseMode, bool aggressive,
                bool accommodateShortKey, bool multithread) {
    std::map<double, string> keyCandidates;
    int increment;
    if (multithread) {
        increment = 2;
    } else {
        increment = 1;
    }
    for (int i = rangeStart; i <= rangeEnd; i += increment) {
        int tryKeyLength = i; // Need to fix keylength. Only works with length greater than 4.
        string keyBuilder = firstNKeyLetters(n1, n, alphabet, formattedCipherText, tryKeyLength);
        string tryKey{};
        if (aggressive && !accommodateShortKey) {
            tryKey = fullKeyStronger(n2, n, alphabet, formattedCipherText, tryKeyLength, keyBuilder);
        } else {
            tryKey = fullKeyNormal(n2, n, alphabet, formattedCipherText, tryKeyLength, keyBuilder);
        }
        double bestScore = n2.score(
                vigenereCipher::decrypt(formattedCipherText, vigenereCipher::formatKey(formattedCipherText, tryKey)));
        keyCandidates[bestScore] = tryKey;
        if (verboseMode) {
            printVerboseResults(bestScore, tryKeyLength, tryKey, originalCipherText, formattedCipherText);
        }
    }
    string key = keyCandidates[keyCandidates.rbegin()->first];
    int keyLength = (int) keyCandidates[keyCandidates.rbegin()->first].length();
    vigenereCipher::setKeyLength(keyLength);
    if (!verboseMode) {
        /*
        if (!multithread) {
            printResults(keyLength, key, originalCipherText, formattedCipherText);
        } else {

        }
         */
        printResults(keyLength, key, originalCipherText, formattedCipherText);
    }
}

/**
 * Return the total elapsed time to run the breakEncryption() function
 * @param startTime the start value for the timer
 * @param breakEncryption the breakEncryption() function
 * @param n1 an nGramScorer object
 * @param n2 an nGramScorer object
 * @param n the number of characters making up the ngram
 * @param rangeStart the minimum key length to try
 * @param rangeEnd the maximum key length to try
 * @param alphabet the English alphabet
 * @param originalCipherText the ciphertext with spaces, punctuation, and non-alphabetic
 * characters
 * @param formattedCipherText all-caps ciphertext with spaces and punctuation removed
 * @param verboseMode specify whether or not to use verbose mode
 * @return the total elapsed time to run the breakEncryption() function
 */
double
totalTimeTaken(std::chrono::time_point<std::chrono::high_resolution_clock> startTime,
               const std::function<void(nGramScorer, nGramScorer, int, int, int, string, string, string,
                                        bool, bool, bool, bool)> &breakEncryption,
               const nGramScorer &n1, const nGramScorer &n2, int n, int rangeStart, int rangeEnd,
               const string &alphabet, string originalCipherText,
               const string &formattedCipherText, bool verboseMode, bool aggressive, bool accommodateShortKey,
               bool multithread) {
    if (multithread) {
        std::thread thread1(breakEncryption, n1, n2, n, rangeStart, rangeEnd, alphabet, originalCipherText,
                            formattedCipherText,
                            verboseMode, aggressive, accommodateShortKey, multithread);
        std::thread thread2(breakEncryption, n1, n2, n, rangeStart + 1, rangeEnd, alphabet, originalCipherText,
                            formattedCipherText,
                            verboseMode, aggressive, accommodateShortKey, multithread);
        thread1.join();
        thread2.join();
    } else {
        breakEncryption(n1, n2, n, rangeStart, rangeEnd, alphabet, std::move(originalCipherText), formattedCipherText,
                        verboseMode, aggressive, accommodateShortKey, false);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    double timeTaken = (double) elapsedTime.count() * 0.001;
    return timeTaken;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Error: invalid number of command line arguments. Please use the following syntax:" << "\n";
        std::cerr << "./vigenereCipherBreaker [ciphertext] [min keylength] [max keylength] [verbose mode]\n" << "\n";
        std::cerr
                << "Example: ./vigenereCipherBreaker \"Uvagxhvrshdm, fu uvagxhaoyq, eg kkw ttrgmxcw sjr jwmha fj "
                   "mtczfeelhk jqi wxrujw ycdpmrktemxof aj hyh hvgjigre gx pvzuv tcixbts.\" 4 20 0"
                << "\n";
        exit(EXIT_FAILURE);
    }
    nGramScorer trigram(std::ifstream(R"(ngrams/trigrams.txt)"));
    nGramScorer quadgram(std::ifstream(R"(ngrams/quadgrams.txt)"));
    string originalCipherText = argv[1];
    string formattedCipherText = formatCiphertext(originalCipherText);
    string alphabet = theAlphabet();
    int rangeStart = std::stoi(argv[2]);
    int rangeEnd = std::stoi(argv[3]);
    bool verboseMode = strcmp(argv[4], "0");
    bool accommodateShortKey;
    bool multithread;
    if (rangeEnd - rangeStart >= 10 && verboseMode) {
        multithread = true;
    } else {
        multithread = false;
    }
    cout << "\nATTEMPTING TO BREAK THE ENCRYPTION AND UNLOCK THE MESSAGE...\n\n";
    auto startTime = std::chrono::high_resolution_clock::now();
    double timeTaken = totalTimeTaken(startTime, breakEncryption, trigram, quadgram, 3, rangeStart, rangeEnd, alphabet,
                                      originalCipherText, formattedCipherText, verboseMode, false, false, multithread);
    char input{};
    cout << "Was the message successfully decrypted? [Y/N] ";
    std::cin >> input;
    if (tolower(input) == 'y') {
        printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
    } else if (tolower(input) == 'n') {
        int keyLength = vigenereCipher::getKeyLength();
        if (keyLength < 12) {
            accommodateShortKey = true;
        } else {
            accommodateShortKey = false;
        }
        cout << "\nEXECUTING A STRONGER ATTEMPT TO BREAK THE ENCRYPTION...\n\n";
        startTime = std::chrono::high_resolution_clock::now();
        timeTaken += totalTimeTaken(startTime, breakEncryption, trigram, quadgram, 3, keyLength, keyLength, alphabet,
                                    originalCipherText, formattedCipherText, false, true, accommodateShortKey, false);
        cout << "Was the message successfully decrypted? [Y/N] ";
        std::cin >> input;
        if (tolower(input) == 'y') {
            printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
        } else if (tolower(input) == 'n') {
            if (keyLength < 15) {
                accommodateShortKey = true;
            } else {
                accommodateShortKey = false;
            }
            cout << "\nEXECUTING AN AGGRESSIVE ATTEMPT TO BREAK THE ENCRYPTION...\n\n";
            startTime = std::chrono::high_resolution_clock::now();
            nGramScorer quintgram(std::ifstream(R"(ngrams/quintgrams.txt)"));
            timeTaken += totalTimeTaken(startTime, breakEncryption, quadgram, quintgram, 4, keyLength, keyLength,
                                        alphabet,
                                        originalCipherText, formattedCipherText, false, true, accommodateShortKey,
                                        false);
            cout << "Was the message successfully decrypted? [Y/N] ";
            std::cin >> input;
            if (tolower(input) == 'y') {
                printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
            } else if (tolower(input) == 'n') {
                if (rangeStart == 4) {
                    rangeStart = 5;
                }
                cout << "\nTRYING ALL KEYS WITHIN SPECIFIED RANGE IN A MORE AGGRESSIVE ATTEMPT...\n\n";
                startTime = std::chrono::high_resolution_clock::now();
                timeTaken += totalTimeTaken(startTime, breakEncryption, quadgram, quintgram, 4, rangeStart, rangeEnd,
                                            alphabet,
                                            originalCipherText, formattedCipherText, false, true, true, multithread);
                cout << "Was the message successfully decrypted? [Y/N] ";
                std::cin >> input;
                if (tolower(input) == 'y') {
                    printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
                } else if (tolower(input) == 'n') {
                    cout
                            << "\nThe properties of the message are such that it is beyond the capabilities of this program to decipher."
                            << "\n";
                    printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
                }
            }
        }
    } else {
        cout << "Invalid response" << "\n";
    }
    return EXIT_SUCCESS;
}

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
    string formattedCiphertext{ciphertext};
    // remove non-alphabetic characters from the ciphertext 
    formattedCiphertext.erase(remove_if(formattedCiphertext.begin(), 
    formattedCiphertext.end(), [](char c){ return !isalpha(c); }), 
    formattedCiphertext.end());
    // convert all characters in the ciphertext string to uppercase
    transform(formattedCiphertext.begin(), formattedCiphertext.end(), 
    formattedCiphertext.begin(), ::toupper);
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
    for (char c : originalFormat) {
        // Check if c is an uppercase or lowercase letter
        if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
            // Check if c is an uppercase letter
            if (c >= 65 && c <= 90) {
                restoredText += (char) toupper(modifiedFormat[j]);
            } else {
                restoredText += (char) tolower(modifiedFormat[j]);
            }
            j++;
        } else {
            restoredText += c;
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
 /**
 Yes, you can use the C++ algorithms library to make this code more efficient. One way to do this is to use the std::sort function to sort the keyCandidates map by score, instead of inserting each new candidate into the map and then returning the key with the highest score. This would avoid the need to iterate over the entire map to find the key with the highest score, which would make the code run faster.
Another way to make the code more efficient is to use the std::vector class instead of the std::map class to store the key candidates. This would allow you to avoid the overhead of maintaining a map, and would make it easier to sort the key candidates by score.
*/
string firstNKeyLetters(nGramScorer ngram, int n, const string &alphabet, const string &ciphertext, int keyLength) {
    std::vector<std::pair<double, string>> keyCandidates;
    string pad(keyLength - n, 'A');
    int totalngramPermutations = pow(26, n);
    for (int i = 0; i < totalngramPermutations; i++) {
        string key = ngramPermutation(n, i, alphabet) + pad;
        string formattedKey = vigenereCipher::formatKey(ciphertext, key);
        string plaintext = vigenereCipher::decrypt(ciphertext, formattedKey);
        double score = 0;
        for (int j = 0; j < (int) ciphertext.length(); j += keyLength) {
            if (j + n < (int) plaintext.length()) {
                score += ngram.score(plaintext.substr(j, n));
            }
        }
        keyCandidates.push_back(std::make_pair(score, key.substr(0, n)));
    }
    // Sort key candidates by score in ascending order
    std::sort(keyCandidates.begin(), keyCandidates.end(),
        [](const std::pair<double, string>& a, const std::pair<double, string>& b) {
            return a.first > b.first;
        });
    return keyCandidates[0].second;
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
    std::vector<std::pair<double, string>> keyCandidates;
    for (int i = 0; i < keyLength - n; i++) {
        keyCandidates.clear();
        for (char c : alphabet) {
            string partialKey = keyBuilder + c;
            string pad(keyLength - partialKey.length(), '\0');
            std::memset(&pad[0], 'A', pad.length());
            string fullKey(partialKey.length() + pad.length(), '\0');
            std::memcpy(&fullKey[0], partialKey.data(), partialKey.length());
            std::memcpy(&fullKey[partialKey.length()], pad.data(), pad.length());
            string fullKeyFormatted = vigenereCipher::formatKey(ciphertext, fullKey);
            string plaintext = vigenereCipher::decrypt(ciphertext, fullKeyFormatted);
            double score = 0;
            for (int j = 0; j < (int) ciphertext.length(); j += keyLength) {
                if (j + partialKey.length() < plaintext.length()) {
                    score += ngram.score(plaintext.substr(j, partialKey.length()));
                }
            }
            keyCandidates.push_back(std::make_pair(score, partialKey));
        }
        std::sort(keyCandidates.begin(), keyCandidates.end(),
        [](const std::pair<double, string>& a, const std::pair<double, string>& b) {
            return a.first > b.first;
        });
        // cout << keyCandidates[0].second << std::endl;
        keyBuilder = keyCandidates[0].second;
    }
    return keyCandidates[0].second;
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
    std::vector<std::pair<double, string>> keyCandidates;
    int increment = (multithread) ? 3 : 1;
    for (int i = rangeStart; i <= rangeEnd; i += increment) {
        int tryKeyLength = i; // Need to fix keylength. Only works with length greater than 4.
        string keyBuilder = firstNKeyLetters(n1, n, alphabet, formattedCipherText, tryKeyLength);
        string tryKey{};
        tryKey = aggressive && !accommodateShortKey ? fullKeyStronger(n2, n, alphabet, formattedCipherText, tryKeyLength, keyBuilder) : 
                                                      fullKeyNormal(n2, n, alphabet, formattedCipherText, tryKeyLength, keyBuilder);
        double bestScore = n2.score(
                vigenereCipher::decrypt(formattedCipherText, vigenereCipher::formatKey(formattedCipherText, tryKey)));
        keyCandidates.push_back(std::make_pair(bestScore, tryKey));
        verboseMode ? printVerboseResults(bestScore, tryKeyLength, tryKey, originalCipherText, formattedCipherText) : (void)0;
    }
    std::sort(keyCandidates.begin(), keyCandidates.end(),
        [](const std::pair<double, string>& a, const std::pair<double, string>& b) {
            return a.first > b.first;
        });
    string key = keyCandidates[0].second;
    int keyLength = key.length();
    vigenereCipher::setKeyLength(keyLength);
    verboseMode ? (void)0 : printResults(keyLength, key, originalCipherText, formattedCipherText);
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
double totalTimeTaken(std::chrono::time_point<std::chrono::high_resolution_clock> startTime,
               const std::function<void(const nGramScorer&, const nGramScorer&, int, int, int, const string&, const string&, const string&,
                                        bool, bool, bool, bool)> &breakEncryption,
               const nGramScorer &n1, const nGramScorer &n2, int n, int rangeStart, int rangeEnd,
               const string &alphabet, const string &originalCipherText,
               const string &formattedCipherText, bool verboseMode, bool aggressive, bool accommodateShortKey,
               bool multithread) {
    if (multithread) {
        std::thread thread1(breakEncryption, std::ref(n1), std::ref(n2), n, rangeStart, rangeEnd, std::ref(alphabet), std::ref(originalCipherText),
                            std::ref(formattedCipherText), verboseMode, aggressive, accommodateShortKey, multithread);
        std::thread thread2(breakEncryption, std::ref(n1), std::ref(n2), n, rangeStart + 1, rangeEnd, std::ref(alphabet), std::ref(originalCipherText),
                            std::ref(formattedCipherText), verboseMode, aggressive, accommodateShortKey, multithread);
        std::thread thread3(breakEncryption, std::ref(n1), std::ref(n2), n, rangeStart + 2, rangeEnd, std::ref(alphabet), std::ref(originalCipherText),
                            std::ref(formattedCipherText), verboseMode, aggressive, accommodateShortKey, multithread);
        thread1.join();
        thread2.join();
        thread3.join();
    } else {
        breakEncryption(n1, n2, n, rangeStart, rangeEnd, alphabet, originalCipherText, formattedCipherText,
                        verboseMode, aggressive, accommodateShortKey, false);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::ratio<1>> elapsedTime = endTime - startTime;
    return elapsedTime.count();
}


void processCommandLineArgs(int argc, char *argv[], string& originalCipherText, string& formattedCipherText, string& alphabet, int& rangeStart, int& rangeEnd, bool& verboseMode) {
    // Check if the correct number of command line arguments were provided
    if (argc != 5) {
        std::cerr << "Error: invalid number of command line arguments. Please use the following syntax:\n"
                  << "./vigenereCipherBreaker [ciphertext] [min keylength] [max keylength] [verbose mode]\n"
                  << "Example: ./vigenereCipherBreaker \"Uvagxhvrshdm, fu uvagxhaoyq, eg kkw ttrgmxcw sjr jwmha fj mtczfeelhk jqi wxrujw ycdpmrktemxof aj hyh hvgjigre gx pvzuv tcixbts.\" 4 20 0\n";
        exit(EXIT_FAILURE);
    }

    // Read command line arguments
    originalCipherText = argv[1];
    formattedCipherText = formatCiphertext(originalCipherText);
    alphabet = theAlphabet();
    rangeStart = std::stoi(argv[2]);
    rangeEnd = std::stoi(argv[3]);
    verboseMode = strcmp(argv[4], "0") != 0;
}

int main(int argc, char *argv[]) {
    // Create n-gram scorers
    nGramScorer trigram(std::ifstream(R"(ngrams/trigrams.txt)"));
    nGramScorer quadgram(std::ifstream(R"(ngrams/quadgrams.txt)"));

    // Read and process command line arguments
    string originalCipherText, formattedCipherText, alphabet;   
    int rangeStart, rangeEnd;
    bool verboseMode;
    processCommandLineArgs(argc, argv, originalCipherText, formattedCipherText, alphabet, rangeStart, rangeEnd, verboseMode);
    
    int numCores = std::thread::hardware_concurrency();
    bool multithread = (rangeEnd - rangeStart >= 10) && (verboseMode) && (numCores > 1);
    cout << "\nATTEMPTING TO BREAK THE ENCRYPTION AND UNLOCK THE MESSAGE...\n\n";
    auto startTime = std::chrono::high_resolution_clock::now();
    double timeTaken = totalTimeTaken(startTime, breakEncryption, trigram, quadgram, 3, rangeStart, rangeEnd, alphabet,
                                      originalCipherText, formattedCipherText, verboseMode, false, false, multithread);
    char response{};
    bool success = false;
    bool exhaustedAttempts = false;
    bool attempts[] = {false, false, false, false};
    while (!success && !exhaustedAttempts) {
        if (!attempts[0]) {
            cout << "Was the message successfully decrypted? [Y/N] ";
            std::cin >> response;
            attempts[0] = true;
        } else if (!attempts[1]) {
            int keyLength = vigenereCipher::getKeyLength();
            bool accommodateShortKey = (keyLength < 12);
            cout << "\nEXECUTING A STRONGER ATTEMPT TO BREAK THE ENCRYPTION...\n\n";
            startTime = std::chrono::high_resolution_clock::now();
            timeTaken += totalTimeTaken(startTime, breakEncryption, trigram, quadgram, 3, keyLength, keyLength, alphabet,
                                        originalCipherText, formattedCipherText, false, true, accommodateShortKey, false);
            cout << "Was the message successfully decrypted? [Y/N] ";
            std::cin >> response;
            attempts[1] = true;
        } else if (!attempts[2]) {
            int keyLength = vigenereCipher::getKeyLength(); // will be incorrect if multithread was on.
            bool accommodateShortKey = (keyLength < 15);
            cout << "\nEXECUTING AN AGGRESSIVE ATTEMPT TO BREAK THE ENCRYPTION...\n\n";
            startTime = std::chrono::high_resolution_clock::now();
            nGramScorer quintgram(std::ifstream(R"(ngrams/quintgrams.txt)"));
            timeTaken += totalTimeTaken(startTime, breakEncryption, quadgram, quintgram, 4, keyLength, keyLength,
                                        alphabet,
                                        originalCipherText, formattedCipherText, false, true, accommodateShortKey,
                                        false);
            cout << "Was the message successfully decrypted? [Y/N] ";
            std::cin >> response;
            attempts[2] = true;
        } else if (!attempts[3]) {
            nGramScorer quintgram(std::ifstream(R"(ngrams/quintgrams.txt)"));
            rangeStart = (rangeStart == 4) ? 5 : rangeStart;
            cout << "\nTRYING ALL KEYS WITHIN SPECIFIED RANGE IN A MORE AGGRESSIVE ATTEMPT...\n\n";
            startTime = std::chrono::high_resolution_clock::now();
            timeTaken += totalTimeTaken(startTime, breakEncryption, quadgram, quintgram, 4, rangeStart, rangeEnd,
                                        alphabet,
                                        originalCipherText, formattedCipherText, false, true, true, multithread);
            cout << "Was the message successfully decrypted? [Y/N] ";
            std::cin >> response;
            attempts[4] = true;
        } else if (!attempts[4]) {
            cout
                << "\nThe properties of the message are such that it is beyond the capabilities of this program to decipher."
                << "\n";
            printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
            attempts[4] = true;
            exhaustedAttempts = true;
        }
        if (tolower(response) == 'y') {
            success = true;
            printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
        }
        if (tolower(response) != 'y' && tolower(response != 'n')) {
            cout << "Invalid response" << "\n";
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

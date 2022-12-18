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
 * Return the English alphabet as a const char*
 * @return the English alphabet
 */
const char* theAlphabet() {
    return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

/**
 * Formats the given ciphertext by removing non-alphabetic characters and converting 
 * it to uppercase.
 * 
 * @param ciphertext The original ciphertext to be formatted.
 * @return The formatted ciphertext.
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
 * 
 * This function generates an n-gram permutation by taking a permutation count and an 
 * alphabet and constructing a string by repeatedly taking the remainder of the permutation 
 * count divided by the length of the alphabet and using the corresponding character from 
 * the alphabet.
 * 
 * @param n The length of the n-gram to generate.
 * @param permutationCount The permutation count to use.
 * @param alphabet The alphabet to use.
 * @return The generated n-gram permutation.
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
    std::vector<std::pair<double, string>> keyCandidates;
    keyCandidates.reserve(17576); // preallocate elements
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
    // Create a vector to store key candidates
    std::vector<std::pair<double, string>> keyCandidates;
    // Reserve space for 26 key candidates
    keyCandidates.reserve(26);
    // Iterate through all possible characters for the next position in the key
    for (int i = 0; i < keyLength - n; i++) {
        // Clear the key candidates vector because alphabet iteration is restarted
        // for the next character in the next position in the key
        keyCandidates.clear();
        for (char c : alphabet) {
            // Create a partial key by appending the current character to the key builder
            string partialKey = keyBuilder + c;
            // Create a pad string filled with null characters
            string pad(keyLength - partialKey.length(), '\0');
            // Fill the pad string with 'A' characters
            std::memset(&pad[0], 'A', pad.length());
            // Create a full key by concatenating the partial key and the pad
            string fullKey(partialKey.length() + pad.length(), '\0');
            std::memcpy(&fullKey[0], partialKey.data(), partialKey.length());
            std::memcpy(&fullKey[partialKey.length()], pad.data(), pad.length());
            // Format the full key for use in decryption
            string fullKeyFormatted = vigenereCipher::formatKey(ciphertext, fullKey);
            // Decrypt the ciphertext using the formatted full key
            string plaintext = vigenereCipher::decrypt(ciphertext, fullKeyFormatted);
            double score = 0;
            // Iterate through the plaintext in blocks of keyLength and compute the score 
            // for each block
            for (int j = 0; j < (int) ciphertext.length(); j += keyLength) {
                // Check that the current block fits within the plaintext
                if (j + partialKey.length() < plaintext.length()) {
                    // Compute the score for the current block and add it to the total score
                    score += ngram.score(plaintext.substr(j, partialKey.length()));
                }
            }
            // Add the score and partial key to the key candidates vector
            keyCandidates.push_back(std::make_pair(score, partialKey));
        }
        // Sort the key candidates by score in descending order
        std::sort(keyCandidates.begin(), keyCandidates.end(),
        [](const std::pair<double, string>& a, const std::pair<double, string>& b) {
            return a.first > b.first;
        });
        // cout << keyCandidates[0].second << std::endl;
        // Set the key builder to the best candidate from the current iteration
        keyBuilder = keyCandidates[0].second;
    }
    // Return the highest scoring key candidate
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
 * @param bestScore the best score achieved in the decryption process.
 * @param tryKeyLength the length of a potential decryption key
 * @param tryKey the key used in the decryption process.
 * @param originalCipherText the ciphertext with spaces, punctuation, and non-alphabetic characters
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
 * @param keyLength the length of a potential decryption key
 * @param key the key used in the decryption process.
 * @param originalCipherText the ciphertext with spaces, punctuation, and non-alphabetic characters
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
 * This function calculates the total time taken to execute a given function by measuring the elapsed 
 * time between the start and end of the function execution. The function can be executed in single-threaded 
 * or multi-threaded mode.
 * 
 * @param startTime The start time to use for measuring the elapsed time.
 * @param breakEncryption The function to execute.
 * @param n1 An n-gram scorer.
 * @param n2 An n-gram scorer.
 * @param n The length of the n-grams to use.
 * @param rangeStart The start of the range of key lengths to try.
 * @param rangeEnd The end of the range of key lengths to try.
 * @param alphabet The alphabet to use.
 * @param originalCipherText The original ciphertext.
 * @param formattedCipherText The formatted ciphertext.
 * @param verboseMode Whether verbose mode is enabled.
 * @param aggressive Whether to use aggressive mode.
 * @param accommodateShortKey Whether to accommodate short keys.
 * @param multithread Whether to execute the function in multi-threaded mode.
 * @return The total time taken to execute the function.
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

/**
 * 
 * This function processes the command line arguments passed to the program and stores the values in the 
 * provided variables. If the correct number of arguments is not provided, it prints an error message and 
 * exits the program with a failure code.
 * 
 * @param argc The number of command line arguments.
 * @param argv An array of strings containing the command line arguments.
 * @param originalCipherText A string to store the original ciphertext.
 * @param formattedCipherText A string to store the formatted ciphertext.
 * @param alphabet A string to store the alphabet.
 * @param rangeStart An integer to store the start of the range of key lengths to try.
 * @param rangeEnd An integer to store the end of the range of key lengths to try.
 * @param verboseMode A boolean to store whether verbose mode is enabled.
 */
void processCommandLineArgs(int argc, char *argv[], string& originalCipherText, string& formattedCipherText, 
                            string& alphabet, int& rangeStart, int& rangeEnd, bool& verboseMode) {
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

/** 
 * This function repeatedly prompts the user for a response until a valid response is entered. 
 * A valid response is a single character, either 'Y' or 'N', in uppercase or lowercase.
 * 
 * @return The user's response as a string.
 */
string getResponse() {
    string response{};
    bool validResponse = false;
    while (!validResponse) {
        std::cout << "Was the message successfully decrypted? [Y/N] ";
        std::getline(std::cin, response);
        if (response.length() != 1) {
            std::cout << "Invalid response.\n";
        } else {
            switch (tolower((char)response[0])) {
                case 'y':
                    validResponse = true;
                    break;
                case 'n':
                    validResponse = true;
                    break;
                default:
                    std::cout << "Invalid response.\n";
                    break;
            }
        }
    }
    return response;
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
    
    // Determine if parallelization could be used
    int numCores = std::thread::hardware_concurrency();
    bool multithread = (rangeEnd - rangeStart >= 10) && (verboseMode) && (numCores > 1);
    
    double timeTaken = 0;
    string response{};
    bool success = false;
    bool attempts[] = {false, false, false, false};
    bool exhaustedAttempts = false;
    
    while (!success && !exhaustedAttempts) {
        if (!attempts[0]) {
            cout << "\nATTEMPTING TO BREAK THE ENCRYPTION AND UNLOCK THE MESSAGE...\n\n";
            auto startTime = std::chrono::high_resolution_clock::now();
            timeTaken += totalTimeTaken(startTime, breakEncryption, trigram, quadgram, 3, rangeStart, rangeEnd, alphabet,
                                              originalCipherText, formattedCipherText, verboseMode, false, false, multithread);
            response = getResponse();
            attempts[0] = true;
        } else if (!attempts[1]) {
            int keyLength = vigenereCipher::getKeyLength(); // will be incorrect if multithread was on.
            bool accommodateShortKey = (keyLength < 12);
            cout << "\nEXECUTING A STRONGER ATTEMPT TO BREAK THE ENCRYPTION...\n\n";
            auto startTime = std::chrono::high_resolution_clock::now();
            timeTaken += totalTimeTaken(startTime, breakEncryption, trigram, quadgram, 3, keyLength, keyLength, alphabet,
                                        originalCipherText, formattedCipherText, false, true, accommodateShortKey, false);
            response = getResponse();
            attempts[1] = true;
        } else if (!attempts[2]) {
            int keyLength = vigenereCipher::getKeyLength();
            bool accommodateShortKey = (keyLength < 15);
            cout << "\nEXECUTING AN AGGRESSIVE ATTEMPT TO BREAK THE ENCRYPTION...\n\n";
            auto startTime = std::chrono::high_resolution_clock::now();
            nGramScorer quintgram(std::ifstream(R"(ngrams/quintgrams.txt)"));
            timeTaken += totalTimeTaken(startTime, breakEncryption, quadgram, quintgram, 4, keyLength, keyLength,
                                        alphabet,
                                        originalCipherText, formattedCipherText, false, true, accommodateShortKey,
                                        false);
            response = getResponse();
            attempts[2] = true;
        } else if (!attempts[3]) {
            nGramScorer quintgram(std::ifstream(R"(ngrams/quintgrams.txt)"));
            rangeStart = (rangeStart == 4) ? 5 : rangeStart;
            cout << "\nTRYING ALL KEYS WITHIN SPECIFIED RANGE IN A MORE AGGRESSIVE ATTEMPT...\n\n";
            auto startTime = std::chrono::high_resolution_clock::now();
            timeTaken += totalTimeTaken(startTime, breakEncryption, quadgram, quintgram, 4, rangeStart, rangeEnd,
                                        alphabet,
                                        originalCipherText, formattedCipherText, false, true, true, multithread);
            response = getResponse();
            attempts[4] = true;
        } else if (!attempts[4]) {
            cout
                << "\nThe properties of the message are such that it is beyond the capabilities of this program to decipher."
                << "\n";
            printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
            attempts[4] = true;
            exhaustedAttempts = true;
        }
        if (response == "y") {
            success = true;
            printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
        }
    }
    return EXIT_SUCCESS;
}

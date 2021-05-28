#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <map>
#include <valarray>
#include "nGramScorer.h"
#include "vigenereCipher.h"

using std::string;
using std::cout;
using std::endl;

string theAlphabet() {
    return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

string formatCiphertext(const string &ciphertext) {
    string formattedCiphertext{};
    for (char c : ciphertext) {
        if (isalpha(c)) {
            formattedCiphertext += toupper(c);
        }
    }
    return formattedCiphertext;
}

string restoreOriginalFormat(const string &originalFormat, string modifiedFormat) {
    string restoredText{};
    int j = 0;
    for (char i : originalFormat) {
        if (isalpha(i)) {
            if (isupper(i)) {
                restoredText += toupper(modifiedFormat[j]);
            } else if (islower(i)) {
                restoredText += tolower(modifiedFormat[j]);
            }
            j++;
        } else {
            restoredText += i;
        }
    }
    return restoredText;
}

// change to nGramPermutations for quadgram modification
string trigramPermutations(int n, string alphabet) {
    string trigram{};
    for (int i = 0; i < 3; i++) {
        trigram += alphabet[n % 26];
        n /= 26;
    }
    return trigram;
}

// rename to firstNKeyLetters for quadgram test
string firstThreeKeyLetters(nGramScorer trigram, const string &alphabet, const string &ciphertext, int keyLength) {
    std::map<double, string> keyCandidates;
    string pad(keyLength - 3, 'A');
    for (int i = 0; i < (int) pow(26, 3); i++) {
        string key = trigramPermutations(i, alphabet) + pad;
        string formattedKey = vigenereCipher::formatKey(ciphertext, key);
        string plaintext = vigenereCipher::decrypt(ciphertext, formattedKey);
        double score = 0;
        for (int j = 0; j < ciphertext.length(); j += keyLength) {
            if (j + 3 < plaintext.length()) {
                score += trigram.score(plaintext.substr(j, 3));
            }
        }
        keyCandidates[score] = key.substr(0, 3);
    }
    return keyCandidates[keyCandidates.rbegin()->first];
}

// add an int n parameter for quintgram test
string
fullKey(nGramScorer quadgram, const string &alphabet, const string &ciphertext, int keyLength, string keyBuilder) {
    std::map<double, string> keyCandidates;
    for (int i = 0; i < keyLength - 3; i++) {
        keyCandidates.clear();
        for (char c : alphabet) {
            string partialKey = keyBuilder + c;
            string pad(keyLength - partialKey.length(), 'A');
            string fullKey = partialKey + pad;
            string fullKeyFormatted = vigenereCipher::formatKey(ciphertext, fullKey);
            string plaintext = vigenereCipher::decrypt(ciphertext, fullKeyFormatted);
            double score = 0;
            for (int j = 0; j < ciphertext.length(); j += keyLength) {
                if (j + partialKey.length() < plaintext.length()) {
                    score += quadgram.score(plaintext.substr(j, partialKey.length()));
                }
            }
            keyCandidates[score] = partialKey;
        }
        //cout << keyCandidates[keyCandidates.rbegin()->first] << endl;
        keyBuilder = keyCandidates[keyCandidates.rbegin()->first];
    }
    return keyCandidates[keyCandidates.rbegin()->first];
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Error: invalid number of command line arguments. Please use the following syntax:" << endl;
        std::cerr << "./vigenereCipherBreaker ciphertext min keylength max keylength verbose mode" << endl;
        exit(EXIT_FAILURE);
    }
    nGramScorer trigram(std::ifstream(R"(..\ngrams\trigrams.txt)"));
    nGramScorer quadgram(std::ifstream(R"(..\ngrams\quadgrams.txt)"));
    string originalCipherText = argv[1];
    string formattedCipherText = formatCiphertext(originalCipherText);
    string alphabet = theAlphabet();
    std::map<double, string> keyCandidates;
    int rangeStart = std::stoi(argv[2]);
    int rangeEnd = std::stoi(argv[3]);
    bool verboseMode = strcmp(argv[4], "0");
    cout << "Attempting to break the encryption and unlock the message...\n" << endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = rangeStart; i < rangeEnd; i++) {
        int keyLength = i; // Need to fix keylength. Only works with length greater than 4.
        string keyBuilder = firstThreeKeyLetters(trigram, alphabet, formattedCipherText, keyLength);
        string theKey = fullKey(quadgram, alphabet, formattedCipherText, keyLength, keyBuilder);
        double bestScore = quadgram.score(
                vigenereCipher::decrypt(formattedCipherText, vigenereCipher::formatKey(formattedCipherText, theKey)));
        keyCandidates[bestScore] = theKey;
        if (verboseMode) {
            cout << "Score: " << std::setprecision(16) << bestScore << ", " << "Key length: " << keyLength << ", "
                 << "Key: "
                 << theKey << "\n" << "Decrypted: "
                 << vigenereCipher::decrypt(formattedCipherText, vigenereCipher::formatKey(formattedCipherText, theKey))
                 << "\n" << endl;
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    auto timeTaken = elapsedTime.count() * 0.001;
    if (!verboseMode) {
        cout << "POTENTIAL MATCH FOUND\n" << endl;
        cout << "KEY LENGTH: " << keyCandidates[keyCandidates.rbegin()->first].length() << endl;
        cout << "KEY: " << keyCandidates[keyCandidates.rbegin()->first] << "\n" << endl;
        cout << "DECRYPTED MESSAGE:" << endl;
        cout << restoreOriginalFormat(originalCipherText, vigenereCipher::decrypt(formattedCipherText,
                                                                                  vigenereCipher::formatKey(
                                                                                          formattedCipherText,
                                                                                          keyCandidates[keyCandidates.rbegin()->first])))
             << "\n" << endl;
    }
    printf("Total elapsed time: %.2f seconds\n", timeTaken);
    return EXIT_SUCCESS;
}
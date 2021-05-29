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
            formattedCiphertext += (char) toupper(c);
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

string ngramPermutations(int n, int permutationCount, string alphabet) {
    string ngram{};
    for (int i = 0; i < n; i++) {
        ngram += alphabet[permutationCount % 26];
        permutationCount /= 26;
    }
    return ngram;
}

string firstNKeyLetters(nGramScorer ngram, int n, const string &alphabet, const string &ciphertext, int keyLength) {
    std::map<double, string> keyCandidates;
    string pad(keyLength - n, 'A');
    for (int i = 0; i < (int) pow(26, n); i++) {
        string key = ngramPermutations(n, i, alphabet) + pad;
        string formattedKey = vigenereCipher::formatKey(ciphertext, key);
        string plaintext = vigenereCipher::decrypt(ciphertext, formattedKey);
        double score = 0;
        for (int j = 0; j < ciphertext.length(); j += keyLength) {
            if (j + n < plaintext.length()) {
                score += ngram.score(plaintext.substr(j, n));
            }
        }
        keyCandidates[score] = key.substr(0, n);
    }
    return keyCandidates[keyCandidates.rbegin()->first];
}

string
fullKey(nGramScorer ngram, int n, const string &alphabet, const string &ciphertext, int keyLength, string keyBuilder) {
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
            for (int j = 0; j < ciphertext.length(); j += keyLength) {
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

void
printResults(int keyLength, const string &key, const string &originalCipherText, const string &formattedCipherText) {
    cout << "POTENTIAL MATCH FOUND\n" << endl;
    cout << "KEY LENGTH: " << keyLength << endl;
    cout << "KEY: " << key << "\n" << endl;
    cout << "DECRYPTED MESSAGE:" << endl;
    cout << restoreOriginalFormat(originalCipherText, vigenereCipher::decrypt(formattedCipherText,
                                                                              vigenereCipher::formatKey(
                                                                                      formattedCipherText, key)))
         << "\n" << endl;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Error: invalid number of command line arguments. Please use the following syntax:" << endl;
        std::cerr << "./vigenereCipherBreaker [ciphertext] [min keylength] [max keylength] [verbose mode]\n" << endl;
        std::cerr
                << "Example: ./vigenereCipherBreaker \"Uvagxhvrshdm, fu uvagxhaoyq, eg kkw ttrgmxcw sjr jwmha fj mtczfeelhk jqi wxrujw ycdpmrktemxof aj hyh hvgjigre gx pvzuv tcixbts.\" 4 20 0"
                << endl;
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
    cout << "ATTEMPTING TO BREAK THE ENCRYPTION AND UNLOCK THE MESSAGE...\n" << endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = rangeStart; i < rangeEnd; i++) {
        int tryKeyLength = i; // Need to fix keylength. Only works with length greater than 4.
        string keyBuilder = firstNKeyLetters(trigram, 3, alphabet, formattedCipherText, tryKeyLength);
        string tryKey = fullKey(quadgram, 3, alphabet, formattedCipherText, tryKeyLength, keyBuilder);
        double bestScore = quadgram.score(
                vigenereCipher::decrypt(formattedCipherText, vigenereCipher::formatKey(formattedCipherText, tryKey)));
        keyCandidates[bestScore] = tryKey;
        if (verboseMode) {
            cout << "Score: " << std::setprecision(16) << bestScore << ", " << "Key length: " << tryKeyLength << ", "
                 << "Key: " << tryKey << "\n"
                 << "Decrypted: "
                 << vigenereCipher::decrypt(formattedCipherText, vigenereCipher::formatKey(formattedCipherText, tryKey))
                 << "\n" << endl;
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    double timeTaken = (double) elapsedTime.count() * 0.001;
    string key = keyCandidates[keyCandidates.rbegin()->first];
    int keyLength = (int) keyCandidates[keyCandidates.rbegin()->first].length();
    if (!verboseMode) {
        printResults(keyLength, key, originalCipherText, formattedCipherText);
    }
    char input{};
    cout << "Was the message successfully decrypted? [Y/N] ";
    std::cin >> input;
    if (tolower(input) == 'y') {
        printf("\nTotal elapsed time for operation: %.2f seconds\n\n", timeTaken);
    } else if (tolower(input) == 'n') {
        // Since key lengths aren't correctly guessed, the whole range of keys will need to be tried
        // Need to create a function for the loop that tries the range of keys
        // Start with the key length candidate from first attempt. Then try all keys if that doesn't work
        cout << "\nEXECUTING A MORE AGGRESSIVE ATTEMPT TO BREAK THE ENCRYPTION...\n" << endl;
        nGramScorer quintgram(std::ifstream(R"(..\ngrams\quintgrams.txt)"));
        string keyBuilder = firstNKeyLetters(quadgram, 4, alphabet, formattedCipherText, keyLength);
        string tryKey = fullKey(quintgram, 4, alphabet, formattedCipherText, keyLength, keyBuilder);
        endTime = std::chrono::high_resolution_clock::now();
        elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        timeTaken = (double) elapsedTime.count() * 0.001;
        printResults(keyLength, tryKey, originalCipherText, formattedCipherText);
        printf("Total elapsed time for operation: %.2f seconds\n\n", timeTaken);
    } else {
        cout << "Invalid response" << endl;
    }
    return EXIT_SUCCESS;
}
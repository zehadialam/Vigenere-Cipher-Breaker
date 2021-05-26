#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <map>
#include <valarray>
#include "nGramScorer.h"
#include "vigenereCipher.h"

using namespace std;

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

string trigramPermutations(int n, string alphabet) {
    string trigram{};
    for (int i = 0; i < 3; i++) {
        trigram += alphabet[n % 26];
        n /= 26;
    }
    return trigram;
}

string firstThreeKeyLetters(nGramScorer trigram, const string &alphabet, const string &ciphertext, int keyLength) {
    map<double, string> keyCandidates;
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

string
fullKey(nGramScorer quadgram, const string &alphabet, const string &ciphertext, int keyLength, string keyBuilder) {
    map<double, string> keyCandidates;
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

int main() {
    nGramScorer trigram(ifstream(R"(..\ngrams\trigrams.txt)"));
    nGramScorer quadgram(ifstream(R"(..\ngrams\quadgrams.txt)"));
    string ciphertext = formatCiphertext(
            "Uvagxhvrshdm, fu uvagxhaoyq, eg kkw ttrgmxcw sjr jwmha fj mtczfeelhk jqi wxrujw ycdpmrktemxof aj hyh hvgjigre gx pvzuv tcixbts.");
    string alphabet = theAlphabet();
    int rangeStart = 4;
    int rangeEnd = 20;
    for (int i = rangeStart; i < rangeEnd; i++) {
        int keyLength = i; // Need to fix keylength. Only works with length greater than 4.
        string keyBuilder = firstThreeKeyLetters(trigram, alphabet, ciphertext, keyLength);
        string theKey = fullKey(quadgram, alphabet, ciphertext, keyLength, keyBuilder);
        double bestScore = quadgram.score(
                vigenereCipher::decrypt(ciphertext, vigenereCipher::formatKey(ciphertext, theKey)));
        cout << "Score: " << setprecision(16) << bestScore << ", " << "Key length: " << keyLength << ", " << "Key: "
             << theKey << "\n" << "Decrypted: "
             << vigenereCipher::decrypt(ciphertext, vigenereCipher::formatKey(ciphertext, theKey))
             << "\n" << endl;
    }
    return EXIT_SUCCESS;
}
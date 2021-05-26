#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <map>
#include <valarray>
#include "nGramScorer.h"

using namespace std;

string formatKey(const string& ciphertext, const string& key) {
    string cipherTextLengthKey{};
    for (int i = 0; i < ciphertext.length(); i += key.length()) {
        cipherTextLengthKey += key;
    }
    return cipherTextLengthKey.substr(0, ciphertext.length());
}

string decrypt(string ciphertext, string key) {
    string decrypted{};
    for (int i = 0; i < ciphertext.length(); i++) {
        decrypted += toupper((((tolower(ciphertext[i] - 97)) - (tolower(key[i] - 97))) % 26 + 26) % 26 + 97);
    }
    return decrypted;
}

string trigramPermutations(int n, string alphabet) {
    string trigram{};
    for (int i = 0; i < 3; i++) {
        trigram += alphabet[n % 26];
        n /= 26;
    }
    return trigram;
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

string theAlphabet() {
    return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

string firstThreeKeyLetters(nGramScorer trigram, const string &alphabet, const string &ciphertext, int keyLength,
                            map<double, string> keyCandidates) {
    string pad(keyLength - 3, 'A');
    for (int i = 0; i < (int) pow(26, 3); i++) {
        string key = trigramPermutations(i, alphabet) + pad;
        string formattedKey = formatKey(ciphertext, key);
        string plaintext = decrypt(ciphertext, formattedKey);
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

string fullKey(nGramScorer qgram, const string &alphabet, const string &ciphertext, int keyLength, string keyBuilder,
               map<double, string> keyCandidates) {
    for (int i = 0; i < keyLength - 3; i++) {
        keyCandidates.clear();
        for (char c : alphabet) {
            string partialKey = keyBuilder + c;
            string pad(keyLength - partialKey.length(), 'A');
            string fullKey = partialKey + pad;
            string fullKeyFormatted = formatKey(ciphertext, fullKey);
            string plaintext = decrypt(ciphertext, fullKeyFormatted);
            double score = 0;
            for (int j = 0; j < ciphertext.length(); j += keyLength) {
                if (j + partialKey.length() < plaintext.length()) {
                    score += qgram.score(plaintext.substr(j, partialKey.length()));
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
    nGramScorer qgram(ifstream(R"(C:\Users\zna58005\CLionProjects\vigenerecipher\quadgrams.txt)"));
    nGramScorer trigram(ifstream(R"(C:\Users\zna58005\CLionProjects\vigenerecipher\trigrams.txt)"));
    string ciphertext = formatCiphertext(
            "Lffcwsyzdtyg, ca qylsxgtrkp, qg cvl cueublgv ibm gahgc gn witpbrebrv jgz vitcfn qvzpyfqfekqcw wu gki hzhwvvqn cm gkmjl seibwng");
    string alphabet = theAlphabet();
    map<double, string> keyCandidates;
    int rangeStart = 4;
    int rangeEnd = 20;
    for (int i = rangeStart; i < rangeEnd; i++) {
        int keyLength = i; // Need to fix keylength. Only works with length greater than 4.
        string keyBuilder = firstThreeKeyLetters(trigram, alphabet, ciphertext, keyLength, keyCandidates);
        string theKey = fullKey(qgram, alphabet, ciphertext, keyLength, keyBuilder, keyCandidates);
        double bestScore = qgram.score(decrypt(ciphertext, formatKey(ciphertext, theKey)));
        cout << "Score: " << setprecision(16) << bestScore << ", " << "Key length: " << keyLength << ", " << "Key: "
             << theKey << "\n" << "Decrypted: " << decrypt(ciphertext, formatKey(ciphertext, theKey))
             << "\n" << endl;
    }
    return EXIT_SUCCESS;
}
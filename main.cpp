#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <map>
//#include <algorithm>
#include <valarray>

#include "nGramScorer.h"

using namespace std;

string generateKey(string str, string key) {
    int x = str.size();
    for (int i = 0;; i++) {
        if (x == i)
            i = 0;
        if (key.size() == str.size())
            break;
        key.push_back(key[i]);
    }
    return key;
}

string originalText(string cipher_text, string key) {
    string orig_text;
    for (int i = 0; i < cipher_text.size(); i++) {
        char x = (cipher_text[i] - key[i] + 26) % 26;
        x += 'A';
        orig_text.push_back(x);
    }
    return orig_text;
}

string trigramPermutations(int n, string arr) {
    string trigram{};
    for (int i = 0; i < 3; i++) {
        trigram += arr[n % 26];
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
    string A(keyLength - 3, 'A');
    for (int i = 0; i < (int) pow(26, 3); i++) {
        string key = trigramPermutations(i, alphabet) + A;
        string key2 = generateKey(ciphertext, key);
        string pt = originalText(ciphertext, key2);
        double score = 0;
        for (int j = 0; j < ciphertext.length(); j += keyLength) { // 13 is the keyBuilder length
            score += trigram.score(pt.substr(j, 3));
        }
        keyCandidates[score] = key.substr(0, 3);
    }
    return keyCandidates[keyCandidates.rbegin()->first];
}

string fullKey(nGramScorer qgram, const string &alphabet, const string &ciphertext, int keyLength, string keyBuilder,
               map<double, string> keyCandidates) {
    for (int i = 0; i < keyLength - 3; i++) { // keyBuilder length - 3
        keyCandidates.clear();
        for (char c : alphabet) {
            string partialKey = keyBuilder + c;
            string pad(keyLength - partialKey.length(), 'A');
            string fullKey = partialKey + pad;
            string fullKey2 = generateKey(ciphertext, fullKey);
            string pt = originalText(ciphertext, fullKey2);
            double score = 0;
            for (int j = 0; j < ciphertext.length(); j += keyLength) {
                if (j + partialKey.length() < pt.length()) {
                    score += qgram.score(pt.substr(j, partialKey.length()));
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
            "Slycmiskhtvp, mh wrlinaevkm, zq jbe ckuompgs rlt mthws ay aiqylykurl zak ziqlpu wozfozbjehzmd cn gay bklwseau if gacdw wefkgum");
    string alphabet = theAlphabet();
    int keyLength = 13;
    map<double, string> keyCandidates;
    string keyBuilder = firstThreeKeyLetters(trigram, alphabet, ciphertext, keyLength, keyCandidates);
    string theKey = fullKey(qgram, alphabet, ciphertext, keyLength, keyBuilder, keyCandidates);
    cout << theKey << endl;
    cout << setprecision(16) << qgram.score(
            "CRYPTOGRAPHYORCRYPTOLOGYISTHEPRACTICEANDSTUDYOFTECHNIQUESFORSECURECOMMUNICATIONINTHEPRESENCEOFTHIRDPARTIES")
         << endl;
    return EXIT_SUCCESS;
}
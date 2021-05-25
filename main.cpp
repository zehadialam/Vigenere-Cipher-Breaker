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

string trigrams(int n, char arr[]) {
    string trigram{};
    for (int i = 0; i < 3; i++) {
        trigram += arr[n % 26];
        n /= 26;
    }
    return trigram;
}

string formatCiphertext(const string& ciphertext) {
    string formattedCiphertext{};
    for (char c : ciphertext) {
        if (isalpha(c)) {
            formattedCiphertext += toupper(c);
        }
    }
    return formattedCiphertext;
}

char * alphabet() {
    char arr[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                  'U', 'V', 'W', 'X', 'Y', 'Z'};
    return arr;
}

int main() {
    nGramScorer qgram(ifstream(R"(C:\Users\zna58005\CLionProjects\vigenerecipher\quadgrams.txt)"));
    nGramScorer trigram(ifstream(R"(C:\Users\zna58005\CLionProjects\vigenerecipher\trigrams.txt)"));
    string ciphertext = formatCiphertext("Slycmiskhtvp, mh wrlinaevkm, zq jbe ckuompgs rlt mthws ay aiqylykurl zak ziqlpu wozfozbjehzmd cn gay bklwseau if gacdw wefkgum");
    //cout << ciphertext << endl;
    cout << setprecision(16) << qgram.score(
            "CRYPTOGRAPHYORCRYPTOLOGYISTHEPRACTICEANDSTUDYOFTECHNIQUESFORSECURECOMMUNICATIONINTHEPRESENCEOFTHIRDPARTIES")
         << endl;
    char arr[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                  'U', 'V', 'W', 'X', 'Y', 'Z'};
    string A(13 - 3, 'A');
    map<double, string> keyCandidates;
    for (int i = 0; i < (int) pow(26, 3); i++) {
        string key = trigrams(i, arr) + A;
        string key2 = generateKey(ciphertext, key);
        string pt = originalText(ciphertext, key2);
        double score = 0;
        for (int j = 0; j < ciphertext.length(); j += 13) { // 13 is the keyBuilder length
            score += trigram.score(pt.substr(j, 3));
        }
        keyCandidates[score] = key.substr(0, 3);
    }
    string partialKey = keyCandidates[keyCandidates.rbegin()->first];
    //cout << partialKey << endl;
    string keyBuilder[] = {partialKey};
    for (int i = 0; i < 10; i++) { // keyBuilder length - 3
        keyCandidates.clear();
        for (char c : arr) {
            partialKey = keyBuilder[0] + c;
            string pad(13 - partialKey.length(), 'A');
            string fullKey = partialKey + pad;
            string fullKey2 = generateKey(ciphertext, fullKey);
            string pt = originalText(ciphertext, fullKey2);
            double score = 0;
            for (int j = 0; j < ciphertext.length(); j += 13) {
                if (j + partialKey.length() < pt.length()) {
                    score += qgram.score(pt.substr(j, partialKey.length()));
                }
            }
            keyCandidates[score] = partialKey;
        }
        //cout << keyCandidates[keyCandidates.rbegin()->first] << endl;
        keyBuilder[0] = keyCandidates[keyCandidates.rbegin()->first];
    }
    cout << keyCandidates[keyCandidates.rbegin()->first] << endl;
    return EXIT_SUCCESS;
}
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <map>
#include <algorithm>
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

int main() {
    nGramScorer qgram(ifstream(R"(C:\Users\zna58005\CLionProjects\vigenerecipher\quadgrams.txt)"));
    nGramScorer trigram(ifstream(R"(C:\Users\zna58005\CLionProjects\vigenerecipher\trigrams.txt)"));
    string s = "Slycmiskhtvp, mh wrlinaevkm, zq jbe ckuompgs rlt mthws ay aiqylykurl zak ziqlpu wozfozbjehzmd cn gay bklwseau if gacdw wefkgum";
    string s2 = {};
    for (char c : s) {
        if (isalpha(c)) {
            s2 += toupper(c);
        }
    }
    //cout << s2 << endl;
    cout << setprecision(16) << qgram.score(
            "CRYPTOGRAPHYORCRYPTOLOGYISTHEPRACTICEANDSTUDYOFTECHNIQUESFORSECURECOMMUNICATIONINTHEPRESENCEOFTHIRDPARTIES")
         << endl;
    char arr[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                  'U', 'V', 'W', 'X', 'Y', 'Z'};
    string A(13 - 3, 'A');
    map<double, string> firstThree;
    for (int i = 0; i < (int) pow(26, 3); i++) {
        string key = trigrams(i, arr) + A;
        string key2 = generateKey(s2, key);
        string pt = originalText(s2, key2);
        double score = 0;
        for (int j = 0; j < s2.length(); j += 13) { // 13 is the key length
            score += trigram.score(pt.substr(j, 3));
        }
        firstThree[score] = key.substr(0, 3);
    }
    string partialKey = firstThree[firstThree.rbegin()->first];
    cout << partialKey << endl;
    string key[] = {partialKey};
    for (int i = 0; i < 10; i++) {
        for (char c : arr) {
            firstThree.clear();
            partialKey = key[0] + c;
            string pad(13 - partialKey.length(), 'A');
            string fullKey = partialKey + pad;
            string fullKey2 = generateKey(s2, fullKey);
            string pt = originalText(s2, fullKey2);
            double score = 0;
            for (int j = 0; j < s2.length(); j += 13) {
                if (j + partialKey.length() < pt.length()) {
                    score += qgram.score(pt.substr(j, partialKey.length()));
                }
            }
            cout << partialKey << " " << score << endl;
            //firstThree[score] = fullKey.substr(0, partialKey.length());
            //partialKey = firstThree[firstThree.rbegin()->first];
        }

    }
    partialKey = firstThree[firstThree.rbegin()->first];
    cout << partialKey << endl;
    return EXIT_SUCCESS;
}
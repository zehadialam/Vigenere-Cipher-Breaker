#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <algorithm>

#include "nGramScorer.h"

using namespace std;

int main() {
    nGramScorer qgram(ifstream (R"(C:\Users\zna58005\CLionProjects\vigenerecipher\quadgrams.txt)"));
    string s = "Slycmiskhtvp, mh wrlinaevkm, zq jbe ckuompgs rlt mthws ay aiqylykurl zak ziqlpu wozfozbjehzmd cn gay bklwseau if gacdw wefkgum";
    string s2={};
    for (char c : s) {
        if (isalpha(c)) {
            s2 += toupper(c);
        }
    }
    cout << setprecision(16) << qgram.score("CRYPTOGRAPHYORCRYPTOLOGYISTHEPRACTICEANDSTUDYOFTECHNIQUESFORSECURECOMMUNICATIONINTHEPRESENCEOFTHIRDPARTIES") << endl;
    return EXIT_SUCCESS;
}
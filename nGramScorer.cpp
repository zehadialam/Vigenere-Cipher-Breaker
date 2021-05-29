#include <cmath>
#include <unordered_map>
#include "nGramScorer.h"

nGramScorer::nGramScorer(std::ifstream file) {
    std::string line{};
    if (file.is_open()) {
        while (getline(file, line)) {
            std::string splitString;
            std::string key;
            std::string value;
            for (char c : line) {
                if (c == ' ') {
                    key = splitString;
                    nGramLength = (int) key.length();
                    splitString = "";
                } else {
                    splitString += c;
                }
            }
            value = splitString;
            sumValues += stoi(value);
            nGramFrequencies[key] = stod(value);
        }
        file.close();
    }
    for (auto &elementPair : nGramFrequencies) {
        nGramFrequencies[elementPair.first] = log10(nGramFrequencies[elementPair.first]) - log10(sumValues);
    }
}

double nGramScorer::score(const std::string &text) {
    double score = 0;
    double floor = log10(0.01) - log10(sumValues);
    for (int i = 0; i < text.length() - nGramLength + 1; i++) {
        if (nGramFrequencies.count(text.substr(i, nGramLength))) {
            score += nGramFrequencies[text.substr(i, nGramLength)];
        } else {
            score += floor;
        }
    }
    return score;
}

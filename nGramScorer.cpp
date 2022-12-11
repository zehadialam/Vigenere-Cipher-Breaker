#include <cmath>
#include <sstream>
#include <unordered_map>
#include "nGramScorer.h"

nGramScorer::nGramScorer(std::ifstream file) {
    std::string line{};
    if (file.is_open()) {
        nGramFrequencies.reserve(389373); // Pre-allocate space for the n-gram frequencies map.
        while (getline(file, line)) {
            std::stringstream lineStream(line);
            std::string ngram;
            int nGramTotalOccurrence;
            lineStream >> ngram >> nGramTotalOccurrence;
            nGramLength = (int) ngram.length();
            sumTotalOccurrence += nGramTotalOccurrence;
            nGramFrequencies[ngram] = nGramTotalOccurrence;
        }
        file.close();
    }
    double logSum = log10(sumTotalOccurrence);
    for (auto &elementPair : nGramFrequencies) {
        nGramFrequencies[elementPair.first] = log10(elementPair.second) - logSum;
    }
}

double nGramScorer::score(const std::string &text) {
    double score = 0;
    double floor = log10(0.01) - log10(sumTotalOccurrence);
    for (int i = 0; i < (int) text.length() - nGramLength + 1; i++) {
        if (nGramFrequencies.count(text.substr(i, nGramLength))) {
            score += nGramFrequencies[text.substr(i, nGramLength)];
        } else {
            score += floor;
        }
    }
    return score;
}

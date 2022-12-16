#include <cmath>
#include <sstream>
#include <unordered_map>
#include "nGramScorer.h"

nGramScorer::nGramScorer(std::ifstream file) {
    std::string line{};
    if (file.is_open()) {
        nGramFrequencies.reserve(389373); // Pre-allocate space for the n-gram frequencies map.
        while (getline(file, line)) {
            // Extract the n-gram and its total occurrence from the line
            std::stringstream lineStream(line);
            std::string ngram;
            int nGramTotalOccurrence;
            lineStream >> ngram >> nGramTotalOccurrence;
            nGramLength = (int) ngram.length();
            sumTotalOccurrence += nGramTotalOccurrence;
            // Store the n-gram frequency in the map
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
    // Get pointers to the start and end of the text
    const char* textStart = text.data();
    const char* textEnd = textStart + text.size();
    // Iterate through each n-gram in the text
    // Continue until current character plus the n-gram length is <= the end of the text.
    for (const char* currentChar = textStart; currentChar <= textEnd - nGramLength; currentChar++) {
        // Look up the n-gram frequency in the map
        auto it = nGramFrequencies.find(std::string(currentChar, currentChar + nGramLength));
        if (it != nGramFrequencies.end()){
            // If the n-gram is found, add its frequency to the score
            score += it->second;
        } else {
            // If the n-gram is not found, add the floor value to the score
            score += floor;
        }
    }
    return score;
}

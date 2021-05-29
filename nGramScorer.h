#include <fstream>

#ifndef NGRAMSCORER_H
#define NGRAMSCORER_H

class nGramScorer {
public:
    explicit nGramScorer(std::ifstream file);

    double score(const std::string &text);

private:
    std::unordered_map<std::string, double> nGramFrequencies;
    int nGramLength;
    double sumValues{};
};

#endif // NGRAMSCORER_H

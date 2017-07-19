#ifndef _INCLUDED_WORD2VEC_NGRAMS_
#define _INCLUDED_WORD2VEC_NGRAMS_

#include "Parameters.h"

namespace Word2Vec
{
    void writeGramVocabularyToWordVectors(std::string const &output_file, Parameters const &params);
    void writeGrams(std::string const &output_file, Parameters const &params);

    /* String to ngram s*/
    int getGrams(std::string const &word, char *gram, size_t index, size_t size, int overlap, int position);

    /* Adds position info to n-gram */
    void addGramPosition(std::string const &word, char *gram, size_t size, size_t index, int position, int overlap);

    /* Utility functions */
    void sumGram(Parameters const &params, size_t offset, std::vector<Parameters::real> &vector);
    void sumFreqGram(Parameters const &params, size_t offset, std::vector<Parameters::real> &vector, size_t cn);
    void minMaxGram(Parameters const &params, size_t offset, std::vector<Parameters::real> &vector, bool min);
    void truncGram(Parameters const &params, size_t offset, std::vector<Parameters::real> &vector, size_t wordLength, size_t gramPos);
}

#endif

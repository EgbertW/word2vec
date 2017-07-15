#ifndef _INCLUDED_WORD2VEC_NGRAMS_
#define _INCLUDED_WORD2VEC_NGRAMS_

#include "Parameters.h"

namespace Word2Vec
{
    void gramVocToWordVec(Vocabulary &voc, Parameters::real *syn0, size_t max_string, size_t layer1_size, int ngram, int group_vec, int binary, int position, int overlap, char *train_file, char *output_file);
    void writeGrams(Vocabulary &voc, Parameters::real *syn0, size_t layer1_size, int ngram, int position, char *output_file, int binary);

    /* String to ngram s*/
    int getGrams(char *word, char *gram, size_t index, size_t size, int overlap, int position);

    /* Adds position info to n-gram */
    void addGramPosition(char *word, char *gram, size_t size, size_t index, int position, int overlap);

    /* Utility functions */
    void sumGram(Parameters::real *syn0, size_t layer1_size, size_t offset, Parameters::real *vector);
    void sumFreqGram(Parameters::real *syn0, size_t layer1_size, size_t offset, Parameters::real *vector, int cn);
    void minmaxGram(Parameters::real *syn0, size_t layer1_size, size_t offset, Parameters::real *vector, int min);
    void truncGram(Parameters::real *syn0, size_t layer1_size, int ngram, size_t offset, Parameters::real *vector, size_t wordLength, int gramPos);
}

#endif

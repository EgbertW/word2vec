#ifndef _INCLUDED_WORD2VEC_NGRAMS_
#define _INCLUDED_WORD2VEC_NGRAMS_

namespace Word2Vec
{
    void gramVocToWordVec(Vocabulary &voc, float *syn0, size_t max_string, size_t layer1_size, int ngram, int hashbang, int group_vec, int binary, int position, int overlap, char *train_file, char *output_file);
    void writeGrams(Vocabulary &voc, float *syn0, size_t layer1_size, int ngram, int hashbang, int position, char *output_file, int binary);

    /*string to ngrams*/
    int getGrams(char *word, char *gram, size_t index, size_t size, int overlap, int position, int hashbang);

    /*adds position info to n-gram*/
    void addGramPosition(char *word, char *gram, size_t size, size_t index, int position, int overlap, int hashbang);


    /* utilit y*/
    void sumGram(float *syn0, size_t layer1_size, size_t offset, size_t *vector);
    void sumFreqGram(float *syn0, size_t layer1_size, size_t offset, float *vector, int cn);
    void minmaxGram(float *syn0, size_t layer1_size, size_t offset, float *vector, int min);
    void truncGram(float *syn0, size_t layer1_size, int ngram, size_t offset, float *vector, size_t wordLength, int gramPos);
}

#endif

#include "ngrams.ih"

namespace Word2Vec
{
    /*Divide vector by ngrams*/
    void truncGram(float *syn0, size_t layer1_size, int ngram, size_t offset, float *vector, size_t wordLength, size_t gramPos)
    {
        //nbGram = wordLength - ngram + 1;
        size_t nbDiv = layer1_size / (wordLength - ngram + 1);

        // if nbdiv*nbGram =! layer1size
        for (size_t i = gramPos * nbDiv; i < layer1_size; ++i)//overrides - make condition cleaner 
            vector[i] = syn0[offset + i];
    }
}

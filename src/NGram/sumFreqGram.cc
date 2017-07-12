#include "ngrams.ih"

namespace Word2Vec
{
    /*group by sum*/
    void sumFreqGram(float *syn0, size_t layer1_size, size_t offset, float *vector, size_t cn)
    {
        for (size_t i = 0; i < layer1_size; ++i)
        {
            vector[i] += (syn0[offset + i]* (1.00 / cn));
        }
    }
}

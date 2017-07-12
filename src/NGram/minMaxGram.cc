#include "ngrams.ih"

namespace Word2Vec
{
    /*1->min 0->max*/
    void minMaxGram(float *syn0, size_t layer1_size, size_t offset, float *vector, bool min)
    {
        if (min)
        {
            for (size_t i = 0; i < layer1_size; ++i)
            {
                if (vector[i] > syn0[offset + i])
                    vector[i] = syn0[offset + i];
            }
        }
        else
        {
            for (size_t i = 0; i < layer1_size; ++i)
            {
                if (vector[i] < syn0[offset + i])
                    vector[i] = syn0[offset + i];
            }
        }
    }
}

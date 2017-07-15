#include "ngrams.ih"

namespace Word2Vec
{
    /*group by sum*/
    void sumGram(Parameters::real *syn0, size_t layer1_size, size_t offset, Parameters::real *vector)
    {
        for (size_t i = 0; i < layer1_size; ++i)
            vector[i] += syn0[offset + i];
    }
}

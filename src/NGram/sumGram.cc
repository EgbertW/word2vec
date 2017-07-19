#include "ngrams.ih"

namespace Word2Vec
{
    /**
     * Group word vector by summing ngram vectors 
     */
    void sumGram(Parameters const &params, size_t offset, std::vector<Parameters::real> &vector)
    {
        for (size_t i = 0; i < params.layer1_size; ++i)
            vector[i] += (*params.syn0)[offset + i];
    }
}

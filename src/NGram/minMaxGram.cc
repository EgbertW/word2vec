#include "ngrams.ih"

namespace Word2Vec
{
    /**
     * Group word vector by taking the minimum or maximum
     */
    void minMaxGram(Parameters const &params, size_t offset, std::vector<Parameters::real> &vector, bool min)
    {
        std::vector<Parameters::real> const &syn0(*params.syn0);
        if (min)
        {
            for (size_t i = 0; i < params.layer1_size; ++i)
            {
                if (vector[i] > syn0[offset + i])
                    vector[i] = syn0[offset + i];
            }
        }
        else
        {
            for (size_t i = 0; i < params.layer1_size; ++i)
            {
                if (vector[i] < syn0[offset + i])
                    vector[i] = syn0[offset + i];
            }
        }
    }
}

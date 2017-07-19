#include "ngrams.ih"

namespace Word2Vec
{
    /**
     * Group word vector by dividing vector by ngrams
     */
    void truncGram(Parameters const &params, size_t offset, std::vector<Parameters::real> &vector, size_t wordLength, size_t gramPos)
    {
        size_t nbDiv = params.layer1_size / (wordLength - params.ngram + 1);

        // if nbdiv*nbGram =! layer1size
        for (size_t i = gramPos * nbDiv; i < params.layer1_size; ++i)//overrides - make condition cleaner 
            vector[i] = (*params.syn0)[offset + i];
    }
}

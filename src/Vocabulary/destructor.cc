#include "vocabulary.ih"

namespace Word2Vec
{
    void Vocabulary::~Vocabulary()
    {
        delete [] vocab_hash;
    }
}

#include "vocabulary.ih"

namespace Word2Vec
{
    Vocabulary::~Vocabulary()
    {
        delete [] d_vocab_hash;
    }
}

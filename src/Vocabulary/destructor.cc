#include "vocabulary.ih"
#include <iostream>

namespace Word2Vec
{
    Vocabulary::~Vocabulary()
    {
        delete [] d_vocab_hash;
    }
}

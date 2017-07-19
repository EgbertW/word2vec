#include "vocabulary.ih"
#include <iostream>

namespace Word2Vec
{
    Vocabulary::~Vocabulary()
    {
        std::cout << "destruct vocab\n";
        delete [] d_vocab_hash;
    }
}

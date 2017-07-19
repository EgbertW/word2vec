#include "vocabulary.ih"

#include <iostream>

namespace Word2Vec
{
    /*Inits a vocabulary*/
    Vocabulary::Vocabulary(size_t vocab_hash_size)
    {
        std::cout << "construct vocab\n";
        d_vocab_hash = new int[vocab_hash_size];
        std::fill(d_vocab_hash, d_vocab_hash + vocab_hash_size, -1);
        d_vocab_hash_size = vocab_hash_size;
        d_train_words = 0;
    }
}

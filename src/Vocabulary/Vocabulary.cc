#include "vocabulary.ih"

#include <iostream>

namespace Word2Vec
{
    /*Inits a vocabulary*/
    Vocabulary::Vocabulary(size_t vocab_hash_size)
    :
        d_vocab_hash_size(vocab_hash_size),
        d_train_words(0),
        d_vocab_hash(vocab_hash_size, Vocabulary::npos)
    {}
}

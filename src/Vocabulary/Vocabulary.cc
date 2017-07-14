#include "vocabulary.ih"

namespace Word2Vec
{
    /*Inits a vocabulary*/
    Vocabulary::Vocabulary(size_t vocab_hash_size)
    {
        d_vocab_hash = new int[vocab_hash_size];
        for (size_t i = 0; i < vocab_hash_size; ++i)
            d_vocab_hash[i] = -1;

        d_vocab_hash_size = vocab_hash_size;
        d_train_words = 0;
    }
}

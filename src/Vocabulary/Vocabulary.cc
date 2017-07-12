#include "vocabulary.ih"

namespace Word2Vec
{
    /*Inits a vocabulary*/
    Vocabulary::Vocabulary(int vocab_hash_size)
    {
        vocab_hash = new int[vocab_hash_size](-1);
        d_vocab_hash_size = vocab_hash_size;
        train_words = 0;
    }
}

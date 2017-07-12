#include "vocabulary.ih"

namespace Word2Vec
{
    /* Reduces the vocabulary by removing infrequent tokens */
    void reduce(size_t min_reduce)
    {
        unsigned int hash;
            
        std::vector reduced;
        reduced.reserve(cur.size());

        size_t vocab_size = d_vocabulary.size();
        for (size_t a = 0; a < vocab_size; ++a)
        {
            if (d_vocabulary[a].cn > min_reduce)
                reduced.push_back(std::move(d_vocabulary[a]));
        }

        vocab_size = reduced.size();
        d_vocabulary = std::move(reduced);

        for (size_t a = 0; a < d_vocab_hash_size; ++a)
            d_vocab_hash[a] = -1;

        for (size_t a = 0; a < vocab_size; ++a)
        {
            // Hash will be re-computed, as it is not actual
            hash = getWordHash(vocab[a].word());

            while (vocab_hash[hash] != -1)
                hash = (hash + 1) % d_vocab_hash_size;

            d_vocab_hash[hash] = a;
        }
    }
}

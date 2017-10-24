#include "vocabulary.ih"

using namespace std;

namespace Word2Vec
{
    /* Reduces the vocabulary by removing infrequent tokens */
    void Vocabulary::reduce()
    {
        static size_t min_reduce = 1;

        auto ptr = d_vocabulary.begin();
        while (ptr != d_vocabulary.end())
        {
            if (ptr->cn() <= min_reduce)
                d_vocabulary.erase(ptr);
            else
                ++ptr;
        }

        fill(d_vocab_hash.begin(), d_vocab_hash.end(), npos);

        size_t s = size();
        for (size_t a = 0; a < s; ++a)
        {
            // Hash will be re-computed, as it is not actual
            size_t hash = getWordHash(get(a).word());

            while (d_vocab_hash[hash] != npos)
                hash = (hash + 1) % d_vocab_hash_size;

            d_vocab_hash[hash] = a;
        }

        ++min_reduce;
    }
}

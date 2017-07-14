#include "vocabulary.ih"

#include <iostream>
#include <algorithm>

using namespace std;

namespace Word2Vec
{
    /* Sorts the vocabulary by frequency using word counts*/
    void Vocabulary::sort(size_t min_count)
    {
        unsigned long long int a,size;
        unsigned int hash;

        if (DEBUG_MODE > 2)
            cout << "Sorting Vocabulary...\n";
        
        // Sort the vocabulary and keep </s> at the first position
        std::sort(d_vocabulary.begin(), d_vocabulary.end());

        for (a = 0; a < d_vocab_hash_size; a++)
            d_vocab_hash[a] = -1;

        size = d_vocabulary.size();
        d_train_words = 0;

        auto ptr = d_vocabulary.begin();
        while (ptr != d_vocabulary.end())
        {
            // Words occuring less than min_count times will be discarded from the vocab
            if (ptr->cn() < min_count)
                ptr = d_vocabulary.erase(ptr);
            else
                ++ptr;
        }

        for (size_t a = 1; a < size; ++a)
        {
            // Hash will be re-computed, as after the sorting it is not actual
            size_t hash = getWordHash(d_vocabulary[a].word());

            while (d_vocab_hash[hash] != -1)
                hash = (hash + 1) % d_vocab_hash_size;

            d_vocab_hash[hash] = a;
            d_train_words += d_vocabulary[a].cn();
        }
    }
}

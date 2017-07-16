#include "vocabulary.ih"

#include <iostream>
#include <algorithm>

using namespace std;

namespace Word2Vec
{
    /* Sorts the vocabulary by frequency using word counts*/
    void Vocabulary::sort(size_t min_count)
    {
        size_t size;
        unsigned int hash;

        if (DEBUG_MODE > 2)
            cout << "Sorting Vocabulary...\n";
        
        // Sort the vocabulary and keep </s> at the first position
        std::sort(d_vocabulary.begin(), d_vocabulary.end());
        cout << "Sorted Vocabulary...\n" << flush;

        for (size_t a = 0; a < d_vocab_hash_size; ++a)
            d_vocab_hash[a] = -1;

        size = d_vocabulary.size();
        d_train_words = 0;

        size_t pre_size = d_vocabulary.size();

        // Words occuring less than min_count times will be discarded from the vocab
        d_vocabulary.erase(
            remove_if(
                d_vocabulary.begin(),
                d_vocabulary.end(), 
                [min_count](VocabularyWord const& e){ return e.cn() < min_count; }
            ),
            d_vocabulary.end()
        );
        size_t erased = pre_size - d_vocabulary.size();
        cout << "Erased " << erased << " under min_county...\n" << flush;
        cout << "Remaiing voc size: " << d_vocabulary.size() << endl;

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

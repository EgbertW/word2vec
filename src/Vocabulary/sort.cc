#include "vocabulary.ih"

#include <iostream>
#include <algorithm>

using namespace std;

namespace Word2Vec
{
    /* Sorts the vocabulary by frequency using word counts*/
    void Vocabulary::sort(size_t min_count)
    {
        if (DEBUG_MODE >= 2)
            cout << "Sorting Vocabulary - stripping values less frequent than " << min_count << "...\n";
        
        // Sort the vocabulary and keep </s> at the first position
        std::stable_sort(d_vocabulary.begin() + 1, d_vocabulary.end());

        fill(d_vocab_hash.begin(), d_vocab_hash.end(), npos);
        d_train_words = 0;

        size_t cnt = 0;
        for (VocabularyWord const &vw : d_vocabulary)
            cnt += vw.cn();

        cout << "Total sum of cn before reduce is: " << cnt << endl;

        // Words occuring less than min_count times will be discarded from the vocab
        d_vocabulary.erase(
            remove_if(
                d_vocabulary.begin() + 1,
                d_vocabulary.end(), 
                [min_count](VocabularyWord const& e){ return e.cn() < min_count; }
            ),
            d_vocabulary.end()
        );

        cout << "Vocab now has size: " << d_vocabulary.size() << endl;

        for (size_t a = 1; a < d_vocabulary.size(); ++a)
        {
            // Hash will be re-computed, as after the sorting it is not actual
            size_t hash = getWordHash(d_vocabulary[a].word());

            while (d_vocab_hash[hash] != npos)
                hash = (hash + 1) % d_vocab_hash_size;

            d_vocab_hash[hash] = a;
            d_train_words += d_vocabulary[a].cn();
        }
        cout << "Trainwords is now: " << d_train_words << endl;
    }
}

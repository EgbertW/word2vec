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
        sort(d_vocabulary.begin(), d_vocabulary.end());

        for (a = 0; a < d_vocab_hash_size; a++)
            d_vocab_hash[a] = -1;

        size = d_vocabulary.size();
        d_train_words = 0;

        for (size_t a = 1; a < size; ++a)
        {
            // Words occuring less than min_count times will be discarded from the vocab
            if (d_vocabulary[a].cn < min_count)
            {
                vocab_size--;
                //free(vocab[vocab_size].word); 
                free(voc->vocab[a].word);
                voc->vocab[a].word = NULL;
            }
            else 
            {
                // Hash will be re-computed, as after the sorting it is not actual
                hash = GetWordHash(voc, voc->vocab[a].word);

                while (voc->vocab_hash[hash] != -1)
                    hash = (hash + 1) % voc->vocab_hash_size;

                voc->vocab_hash[hash] = a;
                voc->train_words += voc->vocab[a].cn;
            }
        }

        voc->vocab = (struct vocab_word *)realloc(voc->vocab, (voc->vocab_size + 1) * sizeof(struct vocab_word));
        if(voc->vocab == NULL)
        {
            printf("memory realloc has miserably failed...\n");
            exit(2);
        }

        // Allocate memory for the binary tree construction
        for (a = 0; a < voc->vocab_size; a++) {
            voc->vocab[a].code = (char *)calloc(MAX_CODE_LENGTH, sizeof(char));
            voc->vocab[a].point = (int *)calloc(MAX_CODE_LENGTH, sizeof(int));
        }
    }
}

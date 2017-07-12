#include "vocabulary.ih"

namespace Word2Vec
{
    /* Returns position of a word in the vocabulary;
     if the word is not found, returns -1*/
    int Vocabulary::search(char const *word)
    {
        int hash = getWordHash(word);
        
        while (true)
        {
            int index = d_vocab_hash[hash];
            if (index == -1)
                return -1;

            if (strcmp(word, d_vocabulary[index].word()) == 0)
                return d_vocab_hash[hash];

            hash = (hash + 1) % d_vocab_hash_size;
        }

        return -1;
    }
}

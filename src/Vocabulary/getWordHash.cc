#include "vocabulary.ih"

#include <iostream>

namespace Word2Vec
{
    /* Returns hash value of a word*/
    int Vocabulary::getWordHash(char const *word) const
    {
        unsigned long long a;
        unsigned long long hash = 0;

        for (size_t a = 0; a < strlen(word); ++a)
            hash = hash * 257 + word[a];

        hash = hash % d_vocab_hash_size;
        return hash;
    }
}


#include "vocabulary.ih"

#include <iostream>

using namespace std;

namespace Word2Vec
{
    /* Returns hash value of a word*/
    size_t Vocabulary::getWordHash(std::string const &word) const
    {
        size_t hash = 0;

        size_t strlen= word.length();
        if (strlen > MAX_STRING)
            strlen = MAX_STRING;
            
        for (size_t a = 0; a < strlen; ++a)
            hash = hash * 257 + word[a];

        hash = hash % d_vocab_hash_size;
        return hash;
    }
}


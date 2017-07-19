#include "vocabulary.ih"

#include <iostream>

using namespace std;

namespace Word2Vec
{
    /* Returns hash value of a word*/
    int Vocabulary::getWordHash(std::string const &word) const
    {
        size_t hash = 0;

        for (size_t a = 0; a < word.length(); ++a)
            hash = hash * 257 + word[a];

        hash = hash % d_vocab_hash_size;
        return hash;
    }
}


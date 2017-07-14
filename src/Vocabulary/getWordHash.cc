#include "vocabulary.ih"

namespace Word2Vec
{
    /* Returns hash value of a word*/
    int Vocabulary::getWordHash(char const *word) const
    {
        unsigned long long a;
        int hash = 0;

        for (size_t a = 0; a < strlen(word); ++a)
            hash = hash * 257 + word[a];

        hash = hash % d_vocabulary.size();
        return hash;
    }
}


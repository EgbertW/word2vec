#include "vocabulary.ih"

using namespace std;

namespace Word2Vec
{
    /* Returns position of a word in the vocabulary;
     if the word is not found, returns -1*/
    size_t Vocabulary::search(string const &word) const
    {
        size_t hash = getWordHash(word);
        
        while (true)
        {
            size_t index = d_vocab_hash[hash];
            if (index == npos)
                return npos;

            if (d_vocabulary[index].word() == word)
                return d_vocab_hash[hash];

            hash = (hash + 1) % d_vocab_hash_size;
        }

        return npos;
    }
}

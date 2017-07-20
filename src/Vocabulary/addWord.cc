#include "vocabulary.ih"

#include <iostream>

using namespace std;

namespace Word2Vec
{
    /** 
     * Adds a word to the vocabulary
     * @return size_t The index of the new word
     */
    size_t Vocabulary::addWord(string const &word)
    {
        size_t hash;

        if (word.length() > MAX_STRING)
            throw runtime_error("Word too long");
        
        VocabularyWord vword(word);
        d_vocabulary.push_back(vword);
        size_t index = this->size() - 1;

        hash = getWordHash(word);
        while (d_vocab_hash[hash] != npos)
            hash = (hash + 1) % d_vocab_hash_size;
        d_vocab_hash[hash] = index;

        return index;
    }
}

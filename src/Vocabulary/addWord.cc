#include "vocabulary.ih"

#include <iostream>

namespace Word2Vec
{
    /** 
     * Adds a word to the vocabulary
     * @return size_t The index of the new word
     */
    size_t Vocabulary::addWord(char const *word)
    {
        size_t hash;
        size_t length = strlen(word) + 1;

        if (length > MAX_STRING)
            length = MAX_STRING;
        
        VocabularyWord vword(word);
        d_vocabulary.push_back(vword);
        size_t index = this->size() - 1;

        hash = getWordHash(word);
        while (d_vocab_hash[hash] != -1)
            hash = (hash + 1) % d_vocab_hash_size;
        d_vocab_hash[hash] = index;

        return index;
    }
}

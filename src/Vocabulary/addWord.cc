#include "vocabulary.ih"

namespace Word2Vec
{
    /** 
     * Adds a word to the vocabulary
     * @return size_t The vocabulary size
     */
    size_t Vocabulary::addWord(char const *word)
    {
        size_t hash;
        size_t length = strlen(word) + 1;

        if (length > MAX_STRING)
            length = MAX_STRING;
        
        VocabularyWord word;
        word.setWord(word);
        d_vocabulary.push_back(word);
        size_t index = d_vocabulary.size() - 1;

        hash = getWordHash(word);
        while (vocab_hash[hash] != -1)
            hash = (hash + 1) % voc->vocab_hash_size;
        vocab_hash[hash] = index;

        return d_vocabulary.size();
    }
}

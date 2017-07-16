#ifndef __WORD2VEC_VOCABULARY_H_INCLUDED_
#define __WORD2VEC_VOCABULARY_H_INCLUDED_

#include <vector>

#include "VocabularyWord.h"

namespace Word2Vec
{
    class Vocabulary
    {
        protected:
            int d_vocab_hash_size;
            unsigned long long int d_train_words;

            int *d_vocab_hash;
            std::vector<VocabularyWord> d_vocabulary;

        public:
            Vocabulary(size_t vocab_hash_size);
            
            /* Destructor */
            ~Vocabulary();

            /* Reads a word from file descriptor fin*/
            void readWord(char *word, std::istream &input);

            /* Reads a word from file descriptor fin*/
            int readWordIndex(std::istream &input);

            /* Returns hash value of a word*/
            int getWordHash(char const *word) const;

            /* Returns position of a word in the vocabulary;
             if the word is not found, returns -1*/
            int search(char const *word);

            /* Reads a word and returns its index in the vocabulary*/
            int readWord(std::istream &input);

            /* Adds a word to the vocabulary*/
            size_t addWord(char const *word);

            /* Sorts the vocabulary by frequency using word counts - EXCEPT 0 -*/
            void sort(size_t min_count);

            /* Reduces the vocabulary by removing infrequent tokens */
            void reduce(size_t min_reduce);

            /*Look if word already in vocab, 
            if not add, if yes, increment. --- REDUCE VOCAB TO 1 /!\  */
            void searchAndAdd(char const *word);

            /*Create a vocab from train file - returns file size*/
            long long readTrainFile(std::string const &train_file, size_t min_count);

            /*Create a vocab of ngram from train file returns file size*/
            long long learnNGramFromFile(std::string const &train_file, size_t min_count, int ngram, int position, bool overlap);

            /*Saves vocab & Occurences*/
            void save(std::string const &save_vocab_file) const;

            /* Reads a saved vocab file */
            long long read(std::string const &read_vocab_file, std::string const &train_file, size_t min_count);

            /* Create binary Huffman tree using the word counts
             Frequent words will have short uniqe binary codes*/
            void createBinaryTree();

            /** Get a word */
            VocabularyWord const &get(size_t index) const;

            /** Get a word */
            VocabularyWord &get(size_t index);

            /** Get the size */
            size_t size() const;

            /** The numer of words to train on */
            size_t nTrainWords() const;

            /** Get the hash size */
            size_t hashSize() const;
    };

    inline VocabularyWord const &Vocabulary::get(size_t index) const
    {
        return d_vocabulary[index];
    }

    inline VocabularyWord &Vocabulary::get(size_t index) 
    {
        return d_vocabulary[index];
    }

    inline size_t Vocabulary::size() const
    {
        return d_vocabulary.size();
    }

    inline size_t Vocabulary::hashSize() const
    {
        return d_vocab_hash_size;
    }

    inline size_t Vocabulary::nTrainWords() const
    {
        return d_train_words;
    }
}
#endif

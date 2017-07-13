#ifndef __WORD2VEC_VOCABULARY_H_INCLUDED_
#define __WORD2VEC_VOCABULARY_H_INCLUDED_

#include <vector>

#define DEBUG_MODE 2
#define MAX_STRING 100
#define MAX_SENTENCE_LENGTH 1000
#define MAX_CODE_LENGTH 40

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
            void readWord(char *word, FILE *fin);

            /* Returns hash value of a word*/
            int getWordHash(char const *word) const;

            /* Returns position of a word in the vocabulary;
             if the word is not found, returns -1*/
            int search(char const *word);

            /* Reads a word and returns its index in the vocabulary*/
            int readWord(std::istream &input);

            /* Adds a word to the vocabulary*/
            int addWord(char const *word);

            /* Sorts the vocabulary by frequency using word counts - EXCEPT 0 -*/
            void sort(size_t min_count);

            /* Reduces the vocabulary by removing infrequent tokens */
            void reduce(size_t min_reduce);

            /*Look if word already in vocab, 
            if not add, if yes, increment. --- REDUCE VOCAB TO 1 /!\  */
            void searchAndAdd(char const *word);

            /*Create a vocab from train file - returns file size*/
            long long readTrainFile(char const * train_file, size_t min_count);

            /*Create a vocab of ngram from train file returns file size*/
            long long learnNGramFromFile(char const *train_file, size_t min_count, bool ngram, bool position, bool overlap);

            /*Saves vocab & Occurences*/
            void save(char const *save_vocab_file) const;

            /* Reads a saved vocab file */
            long long read(char const *read_vocab_file, char const *train_file, size_t min_count);

            /* Create binary Huffman tree using the word counts
             Frequent words will have short uniqe binary codes*/
            void CreateBinaryTree(vocabulary* voc);

            /** Get a word */
            void get(size_t index) const;

            /** Get the size */
            void size() const;

            /** The numer of words to train on */
            void nTrainWords() const;
    };

    inline VocabularyWord const &Vocabulary::get(size_t index) const
    {
        return d_vocabulary[index];
    }

    inline size_t Vocabulary::size() const
    {
        return d_vocabulary.size();
    }

    inline size_t Vocabulary::nTrainWords() const
    {
        return d_train_words;
    }
}
#endif

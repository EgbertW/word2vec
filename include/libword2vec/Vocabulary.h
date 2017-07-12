#ifndef __WORD2VEC_VOCABULARY_H_INCLUDED_
#define __WORD2VEC_VOCABULARY_H_INCLUDED_

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define DEBUG_MODE 2
#define MAX_STRING 100
#define MAX_SENTENCE_LENGTH 1000
#define MAX_CODE_LENGTH 40

namespace Word2Vec
{
    class VocabularyWord
    {
        long long d_cn; // times of occurence in train file
        int *d_point;
        char *d_word;
        char *d_code;
        char d_codelen;
    };

    class Vocabulary
    {
        protected:
            int d_vocab_hash_size;
            unsigned long long int d_train_words;
            long long d_vocab_max_size;
            long long d_vocab_size;
            int *d_vocab_hash;
            struct vocab_word *d_vocab;

        public:
            Vocabulary(size_t vocab_hash_size, size_t vocab_max_size);
            
            /*Reads a word from file descriptor fin*/
            void readWord(char *word, FILE *fin);

            /*Reads a word and adds #hashbangs# around it from file descriptor fin*/
            void readWordHashbang(char *word, FILE *fin);

            /* Returns hash value of a word*/
            int GetWordHash(vocabulary* voc,char *word);

            /*free the vocab structure*/ //TODO
            void DestroyVocab(vocabulary* voc);

            /* Returns position of a word in the vocabulary;
             if the word is not found, returns -1*/
            int SearchVocab(vocabulary* voc,char *word);

            /* Reads a word and returns its index in the vocabulary*/
            int ReadWordIndex(vocabulary* voc,FILE *fin);

            /* Adds a word to the vocabulary*/
            int AddWordToVocab(vocabulary* voc,char *word);

            /* Used for sorting by word counts */
            int VocabCompare(const void *a, const void *b);

            /* Sorts the vocabulary by frequency using word counts - EXCEPT 0 -*/
            void SortVocab(vocabulary* voc, int min_count);

            /* Reduces the vocabulary by removing infrequent tokens */
            void ReduceVocab(vocabulary* voc,  int min_reduce);

            /*Look if word already in vocab, 
            if not add, if yes, increment. --- REDUCE VOCAB TO 1 /!\  */
            void searchAndAddToVocab(vocabulary* voc, char* word);

            /*Create a vocab from train file - returns file size*/
            long long LearnVocabFromTrainFile(vocabulary* voc, char* train_file,int min_count);

            /*Create a vocab of ngram from train file returns file size*/
            long long LearnNGramFromTrainFile(vocabulary* voc, char* train_file,int min_count, int ngram, int hashbang, int position, int overlap);

            /*Saves vocab & Occurences*/
            void SaveVocab(vocabulary* voc, char* save_vocab_file);

            /* Reads a saved vocab file */
            long long ReadVocab(vocabulary* voc, char* read_vocab_file, char* train_file, int min_count);

            /* Create binary Huffman tree using the word counts
             Frequent words will have short uniqe binary codes*/
            void CreateBinaryTree(vocabulary* voc);
    };
}
#endif

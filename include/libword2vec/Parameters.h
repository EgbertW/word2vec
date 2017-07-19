#ifndef __INCLUDED_WORD2VEC_PARAMETERS_
#define __INCLUDED_WORD2VEC_PARAMETERS_

#include <memory>
#include <vector>

#define MAX_SENTENCE_LENGTH 100
#define MAX_CODE_LENGTH 40
#define MAX_STRING 100
#define DEBUG_MODE 2

// Maximum 30 * 0.7 = 21M words in the vocabulary
#define VOCAB_HASH_SIZE 30000000
namespace Word2Vec
{
    // Forward declaration
    class Vocabulary;

    enum TrainType
    {
        CBOW,     // Continuous Bag-of-Words
        CBOWGram, // CBOW with n-grams
        SKIP,     // SKIP 
        SKIPGram  // SKIP with n-grams
    };

    enum GroupType
    {
        NONE = -1,
        SUM = 0,
        MEAN = 1,
        MIN = 2,
        MAX = 3,
        TRUNC = 4,
        WEIGHTEDSUM = 5
    };


    struct Parameters
    {
        typedef float real;

        // The type of training,
        TrainType train_type;

        // The words to train on
        std::shared_ptr<Vocabulary> vocabulary;

        // Training variables

        // Vector table
        std::shared_ptr<std::vector<real> > syn0;

        // Vector table for hierarchical softmax
        std::shared_ptr<std::vector<real> > syn1;

        // Vector table for negative examples?
        std::shared_ptr<std::vector<real> > syn1neg;

        // Pre-computed exponents table
        std::shared_ptr<std::vector<real> > expTable;

        /** Unigram table */
        std::shared_ptr<std::vector<int> > table;

        // Shared parameters

        // Learning rate 
        std::shared_ptr<real> alpha;

        // Actual word count being trained on?
        std::shared_ptr<size_t> word_count_actual;

        // Parameters

        /** Initial learning rate */
        real starting_alpha = 0.025;

        /** Threshold for occurence of word. 0 = no down-sampling. 1e-5 is useful value */
        real sample = 0; 

        /** 0 = no output, 1 = some messages, 2 = frequent updates */
        size_t debug_mode = DEBUG_MODE;

        /** The ID of the thread */
        // TODO: Move to TrainingThread, does not belong here
        size_t threadNumber = 0;

        /** Number of threads used to train */
        size_t num_threads = 1;

        /** Hierarchical Softmax enabled */
        bool hs = 1;

        /** Number of bytes in the training file */
        size_t file_size = 0;

        /** Maximum string length */
        int max_string = MAX_STRING;
        
        /** Table size */
        int exp_table_size = 1000;

        /*** Size of n-grams */
        int ngram = 0;

        /** Minimum amount of occurences of each vocabulary entry */
        size_t min_count = 5;

        int max_exp = 6;
        int window = 5;
        int layer1_size = 100;
        int table_size = 1e8;

        /** Output classes rather than word vectors. 0 = classes disabled, > 0 = number of classes */
        size_t classes = 0;

        /** The number of negative examples; 0 is off, 5-10 are common values */
        int negative = 0;

        /** Add position information to n-grams: indicate where more letters follow by a prefix or suffix of '-' */
        bool position;

        /** Whether ngrams can overlap or not */
        bool overlap;

		/** Computatio of word-vectors with n-grams: -1: No grouping, 0:Sum; 1:Mean; 2:Min; 3:Max; 4:Trunc; 5:FreqSum */
        GroupType group_vec = GroupType::NONE;

        /** Save resulting vectors binary or not */
        // TODO: Why use two formats...
        bool binary = true; 

        /** The input file with the corpus */
        char const *train_file;

        /** When the training started */
        clock_t start;
    };
}
#endif

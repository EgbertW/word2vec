#ifndef __INCLUDED_WORD2VEC_PARAMETERS_
#define __INCLUDED_WORD2VEC_PARAMETERS_

#include <memory>

#define MAX_SENTENCE_LENGTH 100
#define MAX_CODE_LENGTH 40
#define MAX_STRING 100
#define DEBUG_MODE 2

namespace Word2Vec
{
    // Forward declaration
    class Vocabulary;

    enum TrainType
    {
        CBOW,
        CBOWGram,
        SKIP,
        SKIPGram
    };

    struct Parameters
    {
        typedef float real;

        TrainType train_type;

        std::shared_ptr<Vocabulary> vocabulary;

        // Training variables
        real *syn0;
        real *syn1;
        real *syn1neg;
        real *expTable;
        int *table;

        // Shared parameters
        std::shared_ptr<real> alpha;
        std::shared_ptr<size_t> word_count_actual;

        // Parameters
        real starting_alpha = 0.025;
        real sample = 0; 

        size_t debug_mode = DEBUG_MODE;
        size_t threadNumber = 0;
        size_t num_threads = 1;
        int hs = 1;
        size_t file_size = 0;
        int max_string = MAX_STRING;
        int exp_table_size = 1000;
        int ngram = 1;
        int max_exp = 6;
        int window = 5;
        int layer1_size = 100;
        int table_size = 1e8;
        size_t classes = 0;

        bool negative = 0;
        bool position;
        bool overlap;
        bool binary = false; 

        char const *train_file;
        char const *output_file;
        clock_t start;
    };
}
#endif

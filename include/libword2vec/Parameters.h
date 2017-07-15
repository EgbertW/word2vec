#ifndef __INCLUDED_WORD2VEC_PARAMETERS_
#define __INCLUDED_WORD2VEC_PARAMETERS_

#include <memory>

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

        TrainType type;

        std::shared_ptr<Vocabulary> vocabulary;

        // Training variables
        real *syn0;
        real *syn1;
        real *syn1neg;
        real *expTable;
        real *alpha;
        int *table;
        size_t *word_count_actual;

        // Parameters
        real starting_alpha;
        real sample; 

        int d_debug_mode = 2;
        int threadNumber;
        int num_threads;
        int hs = 1;
        int file_size;
        int max_string;
        int exp_table_size;
        int ngram;
        int max_exp;
        int window = 5;
        int layer1_size;
        int table_size;

        bool negative = 0;
        bool position;
        bool overlap;

        char const *train_file;
        clock_t start;

        const int d_table_size = 1e8;
    };
}
#endif

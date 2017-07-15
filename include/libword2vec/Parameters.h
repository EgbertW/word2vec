#ifndef __INCLUDED_WORD2VEC_PARAMETERS_
#define __INCLUDED_WORD2VEC_PARAMETERS_

#include <memory>

#define MAX_SENTENCE_LENGTH 100
#define MAX_CODE_LENGTH 40

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
        real *alpha;
        int *table;
        size_t *word_count_actual;

        // Parameters
        real starting_alpha;
        real sample; 

        int debug_mode = 2;
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
        bool binary; 

        char const *train_file;
        char const *output_file;
        clock_t start;

        const int d_table_size = 1e8;

        size_t classes;
    };
}
#endif

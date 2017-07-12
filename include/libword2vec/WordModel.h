#ifndef __INCLUDED_WORD2VEC_WORDMODEL_H_
#define __INCLUDED_WORD2VEC_WORDMODEL_H_

#define MAX_EXP 6
#define MAX_STRING 100
#define EXP_TABLE_SIZE = 1000;

#include "Vocabulary.h"

namespace Word2Vec
{
    class WordModel
    {
        public:
            typedef float real; // Precision of float numbers
            std::shared_ptr<Vocabulary> d_vocabulary;

            int d_binary = 0;
            int d_cbow = 0;
            int d_debug_mode = 2;
            int d_window = 5,;
            int d_min_count = 5;
            int d_num_threads = 1;
            int d_layer1_size = 100;

            long long d_word_count_actual = 0;
            long long d_file_size = 0;
            long long d_classes = 0;

            real d_alpha = 0.025;
            real d_starting_alpha;
            real d_sample = 0;

            //syn0 = vectors table
            real *d_syn0;
            real *d_syn1;
            real *d_syn1neg
            real *d_expTable;

            clock_t start;

            int d_hs = 1;
            int d_negative = 0;

            const int d_table_size = 1e8;

            int *d_table;
    }
}

#endif


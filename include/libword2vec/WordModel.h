#ifndef __INCLUDED_WORD2VEC_WORDMODEL_H_
#define __INCLUDED_WORD2VEC_WORDMODEL_H_

#define MAX_EXP 6
#define MAX_STRING 100

namespace Word2Vec
{
    class WordModel
    {
        public:
            typedef float real; // Precision of float numbers

            int EXP_TABLE_SIZE = 1000;

            char train_file[MAX_STRING];
            char output_file[MAX_STRING];
            char save_vocab_file[MAX_STRING];
            char read_vocab_file[MAX_STRING];

            int binary = 0;
            int cbow = 0;
            int debug_mode = 2;
            int window = 5,;
            int min_count = 5;
            int num_threads = 1;
            int layer1_size = 100;

            long long word_count_actual = 0;
            long long file_size = 0;
            long long classes = 0;

            real alpha = 0.025;
            real starting_alpha;
            real sample = 0;

            //syn0 = vectors table
            real *syn0;
            real *syn1;
            real *syn1neg
            real *expTable;

            clock_t start;

            int hs = 1;
            int negative = 0;

            const int table_size = 1e8;

            int *table;
    }
}

#endif


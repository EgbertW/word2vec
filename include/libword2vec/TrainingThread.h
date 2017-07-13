#ifndef TRAIN_THREADS
#define TRAIN_THREADS

// TODO: evaluate defines and includes
#define MAX_SENTENCE_LENGTH 100
#define MAX_CODE_LENGTH 40

// Validated includes
#include "WordModel.h"
#include "Vocabulary.h"
#include <thread>
#include <memory>

namespace Word2Vec
{
    class TrainingThread: public std::thread
    {
        public:
            typedef WordModel::real real;

            enum TrainType
            {
                CBOW,
                CBOWGram,
                SKIP,
                SKIPGram
            };

            struct Parameters
            {
                TrainType type;

                std::shared_ptr<Vocabulary> vocabulary;
                real *syn0;
                real *syn1;
                real *syn1neg;
                real *expTable;
                real *alpha;
                real starting_alpha;
                real sample; 

                int threadNumber;
                int num_threads;
                int hs;
                int file_size;
                int max_string;
                int exp_table_size;
                int ngram;
                int max_exp;
                int window;
                int layer1_size;
                int table_size;

                bool negative;
                bool position;
                bool overlap;
                bool hashbang;

                int *table;
                size_t *word_count_actual;
                char const *train_file;
            };
        
        private:
            Parameters d_params;

        public:
            TrainingThread(Parameters parameters)
            :
                d_params(parameters)
            {
                std::thread(&TrainingThread::run, this);  
            }

            ~TrainingThread()
            {
                join();
            }

            void run()
            {
                switch (d_params.type)
                {
                    case CBOW:
                        trainCBOWModel();
                        return;
                    case CBOWGram:
                        trainCBOWModelGram();
                        return;
                    case SKIP:
                        trainSKIPModel();
                        return;
                    case SKIPGram:
                        trainSKIPMOdelGram();
                        return
                    default:
                        throw runtime_error("Invalid train type");
                }
            }

        private:
            void *TrainCBOWModelThread();
            void *TrainCBOWModelThreadGram();
            void *TrainSKIPModelThread();
            void *TrainSKIPModelThreadGram();
        }
    }
}


#endif

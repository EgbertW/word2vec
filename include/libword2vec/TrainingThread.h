#ifndef TRAIN_THREADS
#define TRAIN_THREADS

// TODO: evaluate defines and includes
#define MAX_SENTENCE_LENGTH 100
#define MAX_CODE_LENGTH 40

// Validated includes
#include "WordModel.h"
#include "Vocabulary.h"
#include "Parameters.h"
#include <thread>
#include <memory>

namespace Word2Vec
{
    class TrainingThread: public std::thread
    {
        public:
            typedef WordModel::real real;
        
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

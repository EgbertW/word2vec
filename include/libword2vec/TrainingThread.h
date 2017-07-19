#ifndef TRAIN_THREADS
#define TRAIN_THREADS

// Validated includes
#include "WordModel.h"
#include "Vocabulary.h"
#include "Parameters.h"
#include <thread>
#include <memory>

namespace Word2Vec
{
    class TrainingThread
    {
        friend void WordModel::train();

        public:
            typedef WordModel::real real;
        
        private:
            size_t d_id;
            Parameters d_params;
            std::thread d_thread;

        public:
            ~TrainingThread()
            {
                if (d_thread.joinable())
                    d_thread.join();
            }

            void join()
            {
                if (d_thread.joinable())
                    d_thread.join();
            }

            void run()
            {
                switch (d_params.train_type)
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
                        trainSKIPModelGram();
                        return;
                    default:
                        throw std::runtime_error("Invalid train type");
                }
            }

        private:
            void trainCBOWModel();
            void trainCBOWModelGram();
            void trainSKIPModel();
            void trainSKIPModelGram();

            TrainingThread(size_t id, Parameters parameters)
            :
                d_id(id),
                d_params(parameters)
            {
                d_thread = std::thread(&TrainingThread::run, this);
            }
    };
}


#endif

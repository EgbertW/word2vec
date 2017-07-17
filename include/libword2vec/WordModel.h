#ifndef __INCLUDED_WORD2VEC_WORDMODEL_H_
#define __INCLUDED_WORD2VEC_WORDMODEL_H_

#define MAX_EXP 6
#define MAX_STRING 100
#define EXP_TABLE_SIZE = 1000;

#include "Vocabulary.h"
#include "Parameters.h"
#include <memory>

namespace Word2Vec
{
    class WordModel
    {
        public:
            typedef Parameters::real real;
            Parameters d_params;

        public:
            WordModel(Parameters params);
            ~WordModel();

            void initUnigramTable();
            void train();
    };
}

#endif


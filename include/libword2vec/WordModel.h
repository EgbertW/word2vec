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

            void train();
            void save(std::string const &output_file);

        protected:
            void initUnigramTable();
            void saveTrainResults(std::string const &output_file);
            void saveTrainResultsNgrams(std::string const &output_file);
    };

    inline void WordModel::save(std::string const &output_file)
    {
        if (d_params.ngram > 0)
            saveTrainResultsNgrams(output_file);
        else
            saveTrainResults(output_file);
    }
}

#endif


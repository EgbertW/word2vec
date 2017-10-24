#ifndef __INCLUDED_WORD2VEC_WORDMODEL_H_
#define __INCLUDED_WORD2VEC_WORDMODEL_H_

#include "Vocabulary.h"
#include "Parameters.h"
#include <memory>
#include <utility>

namespace Word2Vec
{
    class WordModel
    {
        public:
            typedef Parameters::real real;
            Parameters &d_params;

            typedef std::pair<real, std::string> WordResult;

        public:
            WordModel(Parameters &params);
            ~WordModel();

            void train();
            void save(std::string const &output_file);
            void readWordModels(std::string const &input_file);
            std::vector<WordResult> findWords(std::string const &input, size_t number_of_results = 40) const;

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


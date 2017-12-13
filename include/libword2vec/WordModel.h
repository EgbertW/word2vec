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

            Vocabulary const &vocab() const;
            std::vector<real> getVector(std::string const &word) const;

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

    inline Vocabulary const &WordModel::vocab() const
    {
        return *d_params.vocabulary;
    }

    inline std::vector<WordModel::real> WordModel::getVector(std::string const &word) const
    {
        size_t vocab_index = (d_params.vocabulary)->search(word);
        if (std::string::npos == vocab_index)
            return std::vector<WordModel::real>();

        std::vector<real> &matrix(*d_params.syn0);
        std::vector<WordModel::real> res(d_params.layer1_size, 0);
        for (size_t a = 0; a < d_params.layer1_size; ++a)
            res[a] += matrix[a + vocab_index * d_params.layer1_size];
        return res;
    }
}

#endif


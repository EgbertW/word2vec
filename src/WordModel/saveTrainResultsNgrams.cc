#include "wordmodel.ih"
#include <libword2vec/NGrams.h>
#include <iostream>

using namespace std;

namespace Word2Vec
{
    void WordModel::saveTrainResultsNgrams(std::string const &output_file)
    {
        if (d_params.debug_mode > 0)
            cout << "Creating word vectors from ngram vectors...\n";

        if (d_params.group_vec != GroupType::NONE)
            writeGramVocabularyToWordVectors(output_file, d_params);
        else
            writeGrams(output_file, d_params);

        if (d_params.debug_mode > 0)
            cout << "Saved word vectors to " << output_file << endl;
    }
}


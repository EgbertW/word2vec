#include "wordmodel.ih"

#include <cmath>

#include <iostream> 
using namespace std;

namespace Word2Vec
{
    void WordModel::initUnigramTable()
    {
        long long train_words_pow = 0;
        real power = 0.75;

        d_params.table = std::make_shared<std::vector<int> >(d_params.table_size);

        for (size_t a = 0; a < d_params.vocabulary->size(); ++a)
            train_words_pow += pow(d_params.vocabulary->get(a).cn(), power); //occurences^power

        size_t i = 0;
        real d1 = pow(d_params.vocabulary->get(i).cn(), power) / (real)train_words_pow; //normalize

        vector<int> &table(*d_params.table);
        for (size_t a = 0; a < d_params.table_size; ++a)
        {
            table[a] = i;

            if (a / (real)d_params.table_size > d1)
            {
                ++i;
                d1 += pow(d_params.vocabulary->get(i).cn(), power) / (real)train_words_pow;
            }

            if (i >= d_params.vocabulary->size())
                i = d_params.vocabulary->size() - 1;
        }
    }
}

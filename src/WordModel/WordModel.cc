#include "wordmodel.ih"
#include <random>
#include <memory>

namespace Word2Vec
{
    void WordModel::WordModel(shared_ptr<Vocabulary> voc)
    :
        d_vocabulary(voc)
    {
        d_params.table = nullptr;
        d_params.syn0 = new real[d_params.layer1_size * d_params.vocabulary->size()];
        d_params.syn1 = d_params.hs ? new real[d_params.layer1_size * d_params.vocabulary->size()](0) : nullptr;
        d_params.syn1neg = d_params.negative > 0 ? new real[d_params.layer1_size * d_params.vocabulary->size()](0) : nullptr;

        random_device rd;
        mt19937 generator(rd);
        uniform_real_distribution<> distribution(-0.5, 0.5);
        for (size_t b = 0; b < d_params.layer1_size; ++b)
            for (size_t a = 0; a < d_params.vocabulary->size(); ++a)
                d_params.syn0[a * d_params.layer1_size + b] = distribution(generator) / d_params.layer1_size;

        d_params.vocabulary->createBinaryTree();

        d_params.expTable = new real[d_params.exp_table_size + 1];
        for (size_t i = 0; i < d_params.exp_table_size; ++i) 
        {
            d_params.expTable[i] = exp((i / (real)d_params.exp_table_size * 2 - 1) * d_params.max_exp); // Precompute the exp() table
            d_params.expTable[i] = d_params.expTable[i] / (d_params.expTable[i] + 1);               // Precompute f(x) = x / (x + 1)
        }
    }
}

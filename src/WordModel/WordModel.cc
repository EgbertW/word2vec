#include "wordmodel.ih"
#include <random>
#include <memory>

namespace Word2Vec
{
    void WordModel::WordModel(shared_ptr<Vocabulary> voc)
    :
        d_vocabulary(voc)
    {
        d_syn0 = new real[layer1_size * d_vocabulary->size()];
        d_syn1 = d_hs ? new real[layer1_size * d_vocabulary->size()](0) : nullptr;
        d_syn1neg = d_negative > 0 ? new real[layer1_size * d_vocabulary->size()](0) : nullptr;

        random_device rd;
        mt19937 generator(rd);
        uniform_real_distribution<> distribution(-0.5, 0.5);
        for (size_t b = 0; b < layer1_size; ++b)
            for (size_t a = 0; a < d_vocabulary->size(); ++a)
                d_syn0[a * layer1_size + b] = distribution(generator) / layer1_size;

        d_vocabulary->createBinaryTree();

        d_expTable = new real[EXP_TABLE_SIZE + 1];
        for (size_t i = 0; i < EXP_TABLE_SIZE; ++i) 
        {
            d_expTable[i] = exp((i / (real)EXP_TABLE_SIZE * 2 - 1) * MAX_EXP); // Precompute the exp() table
            d_expTable[i] = d_expTable[i] / (d_expTable[i] + 1);               // Precompute f(x) = x / (x + 1)
        }
    }
}

#include "wordmodel.ih"
#include <random>
#include <memory>

#include <iostream>
using namespace std;

namespace Word2Vec
{
    WordModel::WordModel(Parameters params)
    :
        d_params(params)
    {
        if (d_params.vocabulary.get() != nullptr)
        {
            size_t units = d_params.layer1_size * d_params.vocabulary->size();
            if (units > 0)
            {
                d_params.syn0 = make_shared<std::vector<real> >(units, 0);
                if (d_params.hs)
                    d_params.syn1 = make_shared<std::vector<real> >(units, 0);
                if (d_params.negative > 0)
                    d_params.syn1neg = make_shared<std::vector<real> >(units, 0);

                random_device r;
                mt19937 generator(r());
                real extent = 0.5 / d_params.layer1_size;
                uniform_real_distribution<> distribution(-extent, extent);
                for (size_t b = 0; b < d_params.layer1_size; ++b)
                    for (size_t a = 0; a < d_params.vocabulary->size(); ++a)
                        (*d_params.syn0)[a * d_params.layer1_size + b] = distribution(generator);

                d_params.vocabulary->createBinaryTree();
            }
        }

        d_params.expTable = make_shared<vector<real> >(d_params.exp_table_size + 1);
        vector<real> &expTable(*d_params.expTable);
        for (size_t i = 0; i < d_params.exp_table_size; ++i) 
        {
            // Precompute sigmoid activation fucntion
            expTable[i] = exp((i / (real)d_params.exp_table_size * 2 - 1) * d_params.max_exp);
            // Precompute f(x) = x / (x + 1)
            expTable[i] = expTable[i] / (expTable[i] + 1);               
        }
    }
}

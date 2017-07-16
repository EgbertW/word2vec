#include "wordmodel.ih"

namespace Word2Vec
{
    WordModel::~WordModel()
    {
        if (d_params.syn0 != nullptr)
        {
            delete [] d_params.syn0;
            d_params.syn0 = nullptr;
        }

        if (d_params.syn1 != nullptr)
        {
            delete [] d_params.syn1;
            d_params.syn1 = nullptr;
        }

        if (d_params.syn1neg != nullptr)
        {
            delete [] d_params.syn1neg;
            d_params.syn1neg = nullptr;
        }

        if (d_params.table != nullptr)
        {
            delete [] d_params.table;
            d_params.table = nullptr;
        }
    }
}

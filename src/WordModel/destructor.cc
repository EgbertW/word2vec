#include "wordmodel.ih"

namespace Word2Vec
{
    WordModel::~WordModel()
    {
        if (d_syn0 != nullptr)
            delete [] d_syn0;

        if (d_syn1 != nullptr)
            delete [] d_syn1;

        if (d_syn1neg != nullptr)
            delete [] d_syn1neg;

        if (d_table != nullptr)
            delete [] d_table;
    }
}

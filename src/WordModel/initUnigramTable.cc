#include "wordmodel.ih"

namespace Word2Vec
{
    void WordModel::initUnigramTable(Vocabulary &voc)
    {
        long long train_words_pow = 0;
        real d1;
        real power = 0.75;
        d_table = new int[table_size];

        for (size_t a = 0; a < d_vocabulary->size(); ++a)
            train_words_pow += pow(d_vocabulary->get(a).cn(), power); //occurences^power

        i = 0;
        d1 = pow(d_vocabulary->get(i).cn(), power) / (real)train_words_pow; //normalize

        for (size_t a = 0; a < table_size; ++a)
        {
            table[a] = i;

            if (a / (real)table_size > d1)
            {
                ++i;
                d1 += pow(d_vocabulary->get(i).cn(), power) / (real)train_words_pow;
            }

            if (i >= d_vocabulary->size())
                i = d_vocabulary->size() - 1;
        }
    }
}

#include "wordmodel.ih"

namespace Word2Vec
{
    void WordModel::InitUnigramTable(Vocabulary &voc)
    {
        int a, i;
        long long train_words_pow = 0;
        real d1, power = 0.75;
        table = (int *)malloc(table_size * sizeof(int));


        for (a = 0; a < voc->vocab_size; a++)
            train_words_pow += pow(voc->vocab[a].cn, power); //occurences^power

        i = 0;
        d1 = pow(voc->vocab[i].cn, power) / (real)train_words_pow; //normalize

        for (a = 0; a < table_size; a++) {

            table[a] = i;

            if (a / (real)table_size > d1) {
              i++;
              d1 += pow(voc->vocab[i].cn, power) / (real)train_words_pow;
            }

            if (i >= voc->vocab_size)
                i = voc->vocab_size - 1;
            }
    }
}

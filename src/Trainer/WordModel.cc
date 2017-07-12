#include "wordmodel.ih"

namespace Word2Vec
{
    void WordModel::WordModel(Vocabulary * voc)
    {
        long long a, b;
        a = posix_memalign((void **)&syn0, 128, (long long)voc->vocab_size * layer1_size * sizeof(real));

        if (syn0 == NULL) {
            printf("Memory allocation failed\n"); 
            exit(1);
        }

        if (hs) {
            a = posix_memalign((void **)&syn1, 128, (long long)voc->vocab_size * layer1_size * sizeof(real));

            if (syn1 == NULL) {
                printf("Memory allocation failed\n");
                exit(1);
            }

            for (b = 0; b < layer1_size; b++)
                for (a = 0; a < voc->vocab_size; a++)
                     syn1[a * layer1_size + b] = 0;
        }

        if (negative>0) {
            a = posix_memalign((void **)&syn1neg, 128, (long long)voc->vocab_size * layer1_size * sizeof(real));

            if (syn1neg == NULL){
                printf("Memory allocation failed\n");
                exit(1);
            }

            for (b = 0; b < layer1_size; b++)
                for (a = 0; a < voc->vocab_size; a++)
                    syn1neg[a * layer1_size + b] = 0;
        }

        for (b = 0; b < layer1_size; b++)
            for (a = 0; a < voc->vocab_size; a++)
                syn0[a * layer1_size + b] = (rand() / (real)RAND_MAX - 0.5) / layer1_size;

        CreateBinaryTree(voc);

        expTable = (real *)malloc((EXP_TABLE_SIZE + 1) * sizeof(real));

        for (size_t i = 0; i < EXP_TABLE_SIZE; i++) 
        {
            expTable[i] = exp((i / (real)EXP_TABLE_SIZE * 2 - 1) * MAX_EXP); // Precompute the exp() table
            expTable[i] = expTable[i] / (expTable[i] + 1);                   // Precompute f(x) = x / (x + 1)
        }
    }
}

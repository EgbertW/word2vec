#include "ngrams.ih"

namespace Word2Vec
{
    int getGrams(char *word, char *gram, size_t index, size_t size, int overlap, int position)
    {
        size_t lenWord = strlen(word);
        size_t lastIndex;

        if (overlap)
            lastIndex = lenWord - size;
        else
            lastIndex = lenWord / size - 1;

        if (index > lastIndex)
            return 0;

        if (lenWord <= size)
            return -1; //usefull for training threads

        int start;

        for (size_t i = 0; i < (size * 2) + 4; ++i) //gram = ngram*2+4 (convention - see vocab.c -> LearnNGramFromTrainFile)
            gram[i] = 0; //reset gram

        if (overlap)
        {
            start = index;
            
            for (size_t i = 0; i < size; ++i)
                gram[i] = word[start + i];

            gram[size] = 0;

            if (position > 0)
                addGramPosition(word, gram, size, index, position, overlap);
        }
        else
        {
            start = index * size;
            size_t i = 0;

            if (index == lastIndex)
            {
                while (word[start + i] != 0)
                {
                    gram[i] = word[start + i];
                    ++i;
                }
                gram[i] = 0;
            }
            else 
            {
                for (size_t i = 0; i < size; ++i)
                    gram[i] = word[start + i];
                gram[size] = 0;
            }

            if (position)
                addGramPosition(word, gram, size, index, position, overlap);
        }

        return 1;
    }
}

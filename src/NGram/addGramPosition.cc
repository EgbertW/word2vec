#include "ngrams.ih"

namespace Word2Vec
{
    /* Adds position to gram Ngram - gram tab size is ngram+3 index: [0->ngram+2]*/
    void addGramPosition(std::string const &word, char *gram, size_t size, size_t index, int position, int overlap)
    {
        size_t gram_length = strlen(gram);
        size_t last_index;

        if (overlap)
            last_index = word.length() - size;
        else
            last_index = word.length() / size - 1;
        
        if (last_index == 0)
            return; 

        if (position)
        {
            // Add a - before and after the ngram if there is no word boundary there
            if (index == 0) // First ngram of word
            {
                gram[size] = '-';
                gram[size + 1] = '\0'; 
                return;
            }

            if (index == last_index) // Last ngram of word
            {
                for (size_t i = gram_length + 1; i > 0; --i)
                    gram[i] = gram[i-1];

                gram[0] = '-';
                return;
            }

            for (size_t i = gram_length + 1; i > 0; --i)
                gram[i] = gram[i - 1];

            gram[0] = '-';
            gram[size + 1] = '-';
            gram[size + 2] = '\0';
        }
        else
        {
            // Adds the gram index before the gram
            if (index == last_index)
                return;

            for (size_t i = gram_length + 1; i >= 0; --i)
                gram[i + 3] = gram[i];
            
            char num[3];
            sprintf(num, "%lu", index);
            if (index >= 10)
            {
                gram[0] = num[0];	
                gram[1] = num[1];
            }
            else
            {
                gram[0] = '0';
                gram[1] = num[0];
            }
            gram[2] = '-';
        }
    }
}

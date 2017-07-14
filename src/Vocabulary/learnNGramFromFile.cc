#include "vocabulary.ih"
#include <fstream>
#include <iostream>

using namespace std;

namespace Word2Vec
{
    /* Create a vocab of ngram from train file */
    long long Vocabulary::learnNGramFromFile(char const *train_file, size_t min_count, int ngram, int position, bool overlap)
    {
        char word[MAX_STRING];
        char *gram = new char[ngram * 2 + 4]; //possibility to merge a ngram with another one < ngram size + position (3 tokens) + '\0'

        for (size_t i = 0; i < d_vocab_hash_size; ++i) //init vocab hashtable
            d_vocab_hash[i] = -1;
        
        ifstream input(train_file, ios_base::in | ios_base::binary);

        if (!input.good())
        {
            cerr << "ERROR: training data file not found!\n";
            exit(1);
        }
        
        d_vocabulary.clear();
        d_train_words = 0;
        addWord("</s>");

        while (!input.eof())
        {
            readWord(word, input);

            size_t lenWord = strlen(word);

            if (lenWord <= ngram)
            { //word smaller or equal to ngram var.
                searchAndAdd(word);
                continue;
            }

            size_t i = 0;
            while (getGrams(word, gram, i, ngram, overlap, position))
            {
                searchAndAdd(gram);
                ++i;
            }

            if (input.eof())
                break;

            ++d_train_words;

            if ((DEBUG_MODE > 1) && (d_train_words % 100000 == 0))
                cout << d_train_words / 1000 << endl;
        }
        cout << "Vocab size: " << d_vocabulary.size() << " - min_count " << min_count << endl;
        sort(min_count);

        if (DEBUG_MODE > 1)
        {
            cout << "Vocabulary size: " << d_vocabulary.size() << endl;
            cout << "Words in train file: " << d_train_words << endl;
        }

        long long file_size = input.tellg();
        input.close();
        delete [] gram;
        return file_size;
    }
}

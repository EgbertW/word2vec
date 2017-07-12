#include "vocabulary.ih"

#include <iostream>
#include <fstream>

using namespace std;

namespace Word2Vec
{
    /*Reads a saved vocabulary file*/
    long long read(char *read_vocab_file, char *train_file, size_t min_count)
    {
        char word[MAX_STRING];

        ifstream input(read_vocab_file, ios_base::in | ios_base::binary);
        if (not input.good())
        {
            cerr << "Vocabulary file not found\n";
            exit(1);
        }

        for (size_t a = 0; a < vocab_hash_size; ++a)
            d_vocab_hash[a] = -1;

        d_vocabulary.clear();
        size_t i = 0;
        while (not input.eof())
        {
            readWord(word, input);

            if (input.eof())
                break;

            a = addWord(word);
            input >> vocab[a].cn;
            ++i;
        }

        sort(min_count);

        if (DEBUG_MODE > 1)
        {
            cout << "Vocabulary size: " << vocab_size << endl;
            cout << "Words in train file: " << train_words << endl;
        }

        input = ifstream(train_file, ios_base::in  || ios_base::binary);
        if (not input.good())
        {
            cerr << "ERROR: training data file not found!\n";
            exit(1);
        }

        input.seekg(0, io_base::end);
        long long file_size = input.tellg();
        input.close();
        return file_size;
    }
}

#include "vocabulary.ih"

#include <iostream>
#include <fstream>

using namespace std;

namespace Word2Vec
{
    /*Reads a saved vocabulary file*/
    long long Vocabulary::read(string const &read_vocab_file, string const &train_file, size_t min_count)
    {
        char word[MAX_STRING];

        ifstream input(read_vocab_file, ios_base::in | ios_base::binary);
        if (not input.good())
        {
            cerr << "Vocabulary file not found\n";
            exit(1);
        }

        for (size_t a = 0; a < d_vocab_hash_size; ++a)
            d_vocab_hash[a] = -1;

        d_vocabulary.clear();
        size_t i = 0;
        while (not input.eof())
        {
            readWord(word, input);

            if (input.eof())
                break;

            size_t a = addWord(word);
            size_t cn;
            input >> cn;
            get(a).setCn(cn);
            ++i;
        }

        sort(min_count);

        if (DEBUG_MODE > 1)
        {
            cout << "Vocabulary size: " << size() << endl;
            cout << "Words in train file: " << nTrainWords() << endl;
        }

        input = ifstream(train_file, ios_base::in | ios_base::binary);
        if (not input.good())
        {
            cerr << "ERROR: training data file not found!\n";
            exit(1);
        }

        input.seekg(0, ios_base::end);
        long long file_size = input.tellg();
        input.close();
        return file_size;
    }
}

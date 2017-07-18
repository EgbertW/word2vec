#include "vocabulary.ih"
#include <fstream>
#include <iostream>

using namespace std;

namespace Word2Vec
{
    /*Create a vocab from train file*/
    long long Vocabulary::readTrainFile(string const &train_file, size_t min_count)
    {
        char word[MAX_STRING];

        ifstream input(train_file, ios_base::in | ios_base::binary);

        if (not input.good())
        {
            // TODO: Throw exception, not exit
            cerr << "ERROR: training data file not found!\n";
            exit(1);
        }

        // Determine file size
        input.seekg(0, ios_base::end);
        size_t file_size = input.tellg();

        // Seek back to the beginning
        input.seekg(0, ios_base::beg);

        // Reset vocabulary
        fill(d_vocab_hash, d_vocab_hash + d_vocab_hash_size, -1);
        d_vocabulary.clear();
        d_train_words = 0;

        addWord("</s>");

        while (true) //not input.eof())
        {
            word[0] = 0;
            readWord(word, input);
            if (word[0] != 0)
                searchAndAdd(word);
            
            if (input.eof())
                break;

            ++d_train_words;

            if ((DEBUG_MODE > 1) && (d_train_words % 100000 == 0))
                cout << d_train_words / 1000 << "K\r" << flush;
        }
        input.close();

        sort(min_count);

        if (DEBUG_MODE > 1)
        {
            cout << "Vocabulary size: " << d_vocabulary.size() << endl;
            cout << "Words in train file:  " << d_train_words << endl;
        }

        return file_size;
    }
}

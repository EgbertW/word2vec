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
            // TODO: Throw exception, not exit
            cerr << "Vocabulary file not found\n";
            exit(1);
        }

        // Reset vocabulary
        fill(d_vocab_hash, d_vocab_hash + d_vocab_hash_size, -1);
        d_vocabulary.clear();

        while (true)
        {
            readWord(word, input);

            if (input.eof())
                break;

            size_t a = addWord(word);
            size_t cn;
            input >> cn;
            char eol = input.get();
            if (eol != 10)
            {
                cout << "Unexpected character " << eol << endl;
                exit(1);
            }

            get(a).setCn(cn);
        }

        sort(min_count);

        size_t total = 0;
        for (VocabularyWord &w : d_vocabulary)
            total += w.cn();

        if (DEBUG_MODE > 1)
        {
            cout << "Vocabulary size: " << d_vocabulary.size() << endl;
            cout << "Words in train file:  " << d_train_words << endl;
            cout << "Sum of word counts:  " << total << endl;
        }

        input = ifstream(train_file, ios_base::in | ios_base::binary);
        if (not input.good())
        {
            cerr << "ERROR: training data file not found!\n";
            exit(1);
        }

        input.seekg(0, ios_base::end);
        long long file_size = input.tellg();
        cout << "FIle size: " << file_size << endl;
        input.close();
        return file_size;
    }
}

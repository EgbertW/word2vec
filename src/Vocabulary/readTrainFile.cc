#include "vocabulary.ih"
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace Word2Vec
{
    /**
     * Create a vocabulary from train file
     */
    size_t Vocabulary::readTrainFile(Parameters const &params)
    {
        ifstream input(params.train_file, ios_base::in | ios_base::binary);

        if (not input.good())
            throw runtime_error("Training data file not found");

        if (params.debug_mode > 0)
            cout << "Reading words from " << params.train_file << endl;

        // Determine file size
        input.seekg(0, ios_base::end);
        size_t file_size = input.tellg();

        // Seek back to the beginning
        input.seekg(0, ios_base::beg);

        // Reset vocabulary
        fill(d_vocab_hash.begin(), d_vocab_hash.end(), npos);
        d_vocabulary.clear();
        d_train_words = 0;

        addWord("</s>");

        while (true) 
        {
            string word;
            readWord(word, input);
            if (not word.empty())
                searchAndAdd(word);
            
            if (input.eof())
                break;

            ++d_train_words;

            if ((DEBUG_MODE > 1) && (d_train_words % 100000 == 0))
                cout << d_train_words / 1000 << "K\r" << flush;
        }
        input.close();

        sort(params.min_count);

        if (DEBUG_MODE > 1)
        {
            cout << "Vocabulary size: " << d_vocabulary.size() << endl;
            cout << "Words in train file:  " << d_train_words << endl;
        }

        return file_size;
    }
}

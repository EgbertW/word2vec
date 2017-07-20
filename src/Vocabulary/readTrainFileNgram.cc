#include "vocabulary.ih"
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace Word2Vec
{
    /** 
     * Create a vocab of ngram from train file
     */
    size_t Vocabulary::readTrainFileNgram(Parameters const &params)
    {
        char *gram = new char[params.ngram * 2 + 4]; //possibility to merge a ngram with another one < ngram size + position (3 tokens) + '\0'

        // Reset hash table
        fill(d_vocab_hash.begin(), d_vocab_hash.end(), npos);

        ifstream input(params.train_file, ios_base::in | ios_base::binary);

        if (!input.good())
            throw runtime_error("Training data file not found");

        if (params.debug_mode > 0)
            cout << "Reading n-grams from " << params.train_file << endl;
        
        // Determine file size
        input.seekg(0, ios_base::end);
        size_t file_size = input.tellg();

        // Seek back to the beginning
        input.seekg(0, ios_base::beg);

        d_vocabulary.clear();
        d_train_words = 0;
        addWord("</s>");

        while (!input.eof())
        {
            string word;
            readWord(word, input);

            if (word.empty())
                continue;

            if (word.length() <= params.ngram)
            { // Word smaller or equal to ngram var.
                searchAndAdd(word);
                continue;
            }

            size_t i = 0;
            while (getGrams(word, gram, i, params.ngram, params.overlap, params.position))
            {
                searchAndAdd(gram);
                ++i;
            }

            ++d_train_words;

            if ((params.debug_mode > 1) && (d_train_words % 100000 == 0))
                cout << d_train_words / 1000 << "K\r" << flush;
        }
        sort(params.min_count);

        if (params.debug_mode > 1)
        {
            cout << "Vocabulary size: " << d_vocabulary.size() << endl;
            cout << "Words in train file: " << d_train_words << endl;
        }

        input.close();
        delete [] gram;
        return file_size;
    }
}

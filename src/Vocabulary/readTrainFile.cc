#include "vocabulary.ih"
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <boost/format.hpp>

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

        size_t newline = addWord("</s>");

        bool first_word = true;
        string last_word;
        while (not input.eof()) 
        {
            string word;
            readWord(word, input);
            if (word.empty())
                continue;

            size_t index = npos;
            index = searchAndAdd(word);

            if (index == newline)
            {
                first_word = true;
                continue;
            }
            else
                first_word = false;
            
            ++d_train_words;

            if ((DEBUG_MODE > 1) && (d_train_words % 100000 == 0))
                cout << boost::format("Words processed: %luK\tVocabulary size: %luK\r") % (d_train_words / 1000) % (size() / 1000) << flush;

            if (not params.train_phrase || first_word)
                continue;

            string bigram_word = (boost::format("%s_%s") % last_word % word).str();
            last_word = std::move(word);
            searchAndAdd(bigram_word);
        }
        input.close();

            cout << "SORTING " << size() << endl;
        sort(params.min_count);
            cout << "SORTED " << size() << endl;

        if (DEBUG_MODE > 1 || true)
        {
            cout << "\nVocabulary size: " << d_vocabulary.size() << endl;
            cout << "Words in train file:  " << d_train_words << endl;
        }

        return file_size;
    }
}

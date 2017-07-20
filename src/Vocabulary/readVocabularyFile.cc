#include "vocabulary.ih"

#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace std;

namespace Word2Vec
{
    /**
     * Reads a saved vocabulary file
     */
    size_t Vocabulary::readVocabularyFile(std::string const &read_vocab_file, Parameters const &params)
    {
        ifstream input(read_vocab_file, ios_base::in | ios_base::binary);
        if (not input.good())
            throw runtime_error("Vobabulary file not found");

        if (params.debug_mode > 0)
            cout << "Reading vocabulary from " << read_vocab_file << endl;

        // Reset vocabulary
        fill(d_vocab_hash.begin(), d_vocab_hash.end(), npos);
        d_vocabulary.clear();

        while (true)
        {
            string word;
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

        sort(params.min_count);

        if (DEBUG_MODE > 1)
        {
            cout << "Vocabulary size: " << d_vocabulary.size() << endl;
            cout << "Words in train file:  " << d_train_words << endl;
        }

        input = ifstream(params.train_file, ios_base::in | ios_base::binary);
        if (not input.good())
            throw runtime_error("Training data file not found");

        input.seekg(0, ios_base::end);
        size_t file_size = input.tellg();
        input.close();
        return file_size;
    }
}

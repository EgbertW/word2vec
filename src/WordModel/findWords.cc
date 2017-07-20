#include "wordmodel.ih"
#include <memory>
#include <sstream>
#include <iostream>
#include <cmath>

using namespace std;

namespace Word2Vec
{
    vector<WordModel::WordResult> WordModel::findWords(string const &input, size_t number_of_results) const
    {
        vector<WordResult> result;
        result.reserve(number_of_results);

        Vocabulary &voc(*d_params.vocabulary);
        vector<string> input_words;
        vector<size_t> input_indexes;
        istringstream tokenize(input);

        vector<real> &matrix(*d_params.syn0);

        while (not tokenize.eof())
        {
            string word;
            voc.readWord(word, tokenize);
            int index = voc.search(word);
            if (index < 0)
            {
                if (d_params.debug_mode > 1)
                    cout << "\nOut of dictionary word: " << word << endl;
                continue;
            }

            input_words.push_back(word);
            input_indexes.push_back(index);
            if (d_params.debug_mode > 1)
                cout << "Word: " << word << "  Position in vocabulary: " << index << endl;
        }

        if (input_indexes.empty())
            return result;

        vector<real> vec(d_params.layer1_size, 0);
        for (size_t vocab_index : input_indexes)
        {
            for (size_t a = 0; a < d_params.layer1_size; ++a)
                vec[a] += matrix[a + vocab_index * d_params.layer1_size];
        }

        real len = 0;
        for (real r : vec)
            len += r * r;
        len = sqrt(len);

        for (real &r : vec)
            r /= len;

        for (size_t vocab_index = 0; vocab_index < voc.size(); ++vocab_index)
        {
            bool is_input = false;
            for (size_t input_index : input_indexes)
            {
                if (input_index == vocab_index)
                    is_input = true;
            }

            if (is_input)
                continue;
                    
            real dist = 0;
            for (size_t a = 0; a < d_params.layer1_size; ++a)
                dist += vec[a] * matrix[a + vocab_index * d_params.layer1_size];

            WordResult cur_word(dist, voc.get(vocab_index).word());

            auto ptr = result.begin();
            for ( ; ptr != result.end() && dist <= ptr->first; ++ptr)
                ;
            
            if (ptr != result.end() || result.size() < number_of_results)
                result.insert(ptr, cur_word);

            if (result.size() > number_of_results)
                result.erase(result.begin() + number_of_results, result.end());
        }

        return std::move(result);
    }
}

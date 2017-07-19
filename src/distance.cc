// Heavily modified, C++ ported version of distance.
// Modifications copyright 2017 - Egbert van der Wal
//
// Original copyright notice:
//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <boost/format.hpp>

#include <libword2vec/Parameters.h>
#include <libword2vec/WordModel.h>

using namespace std;
using namespace Word2Vec;

typedef Parameters::real real;

const long long NUM_WORDS = 40;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n";
        return 0;
    }

    string file_name(argv[1]);
    Parameters params;
    params.debug_mode = 2;
    WordModel model(params);
    model.readWordModels(file_name);

    vector<real> &matrix(*params.syn0);
    Vocabulary &voc(*params.vocabulary);
    vector<real> bestd;
    vector<string> bestw;

    while (true)
    {
        printf("Enter word or sentence (EXIT to break): ");

        string line;
        getline(cin, line);

        if (line == "EXIT")
            break;

        size_t cn = 0;
        size_t b = 0;
        size_t c = 0;

        vector<string> input_words;
        vector<size_t> input_indexes;
        istringstream tokenize(line);

        while (not tokenize.eof())
        {
            string word;
            voc.readWord(word, tokenize);
            int index = voc.search(word);
            if (index < 0)
            {
                cout << "\nOut of dictionary word: " << word << endl;
                continue;
            }

            input_words.push_back(word);
            input_indexes.push_back(index);
            cout << "Word: " << word << "  Position in vocabulary: " << index << endl;
        }

        if (input_indexes.empty())
            continue;

        cout << "\n                                              Word       Cosine distance\n"
            "------------------------------------------------------------------------\n";

        
        vector<real> vec(params.layer1_size, 0);
        for (size_t vocab_index : input_indexes)
        {
            for (size_t a = 0; a < params.layer1_size; ++a)
                vec[a] += matrix[a + vocab_index * params.layer1_size];
        }

        real len = 0;
        for (real r : vec)
            len += r * r;
        len = sqrt(len);

        for (real &r : vec)
            r /= len;

        bestd.clear();
        bestw.clear();

        real e_best_d = 0;
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
            for (size_t a = 0; a < params.layer1_size; ++a)
                dist += vec[a] * matrix[a + vocab_index * params.layer1_size];

            auto ptr = bestd.begin();
            size_t cnt = 0;
            for ( ; ptr != bestd.end(); ++ptr)
            {
                if (dist > *ptr)
                    break;
                ++cnt;
            }
            
            if (ptr != bestd.end() || bestd.size() < NUM_WORDS)
            {
                bestd.insert(ptr, dist);
                bestw.insert(bestw.begin() + cnt, voc.get(vocab_index).word());
            }

            if (bestd.size() > NUM_WORDS)
            {
                bestd.erase(bestd.begin() + NUM_WORDS, bestd.end());
                bestw.erase(bestw.begin() + NUM_WORDS, bestw.end());
            }
        }

        for (size_t a = 0; a < bestd.size(); ++a)
            cout << (boost::format("%50s\t\t%f\n") % bestw[a] % bestd[a]);
    }

    return 0;
}

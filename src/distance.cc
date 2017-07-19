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
    ifstream input(file_name, ios_base::in | ios_base::binary);

    if (!input.good())
    {
        cerr << "Input file not found\n";
        return 1;
    }

    string header;
    getline(input, header); 
    header += ' ';

    Parameters params;
    size_t words;
    params.ngram = 0;
    

    size_t part = 0;
    bool read_some = false;

    string buf;

    for (char ch : header)
    {
        if (ch == ' ')
        {
            if (read_some)
            {
                if (part == 0)
                    words = stoul(buf);
                else if (part == 1)
                    params.layer1_size = stoul(buf);
                else if (part == 2)
                    params.ngram = stoi(buf);
                else if (part == 4)
                    params.position = stoi(buf) != 0;

                buf.clear();
                read_some = false;
                ++part;
            }
            continue;
        }

        buf += ch;
        read_some = true;
    }

    cout << "Reading dictionary\n"
         << "  - Number of words: " << words << "\n"
            "  - Number of hidden units: " << params.layer1_size << "\n";

    if (params.ngram > 0)
    {
        cout << 
            "  - Size of n-grams: " << params.ngram << "\n"
            "  - Position markers: " << (params.position ? "yes" : "no") << endl; 
    }
    cout << endl;

    vector<string> vocabulary(words);
    vector<real> matrix(words * params.layer1_size);

    for (size_t b = 0; b < words; ++b)
    {
        input >> vocabulary[b];
        input.get(); // White space
        if (input.eof())
            throw runtime_error("Invalid file structure");

        for (size_t a = 0; a < params.layer1_size; a++)
        {
            char buf[sizeof(real)];
            input.read(buf, sizeof(real));
            matrix[a + b * params.layer1_size] = (*reinterpret_cast<real *>(buf));
        }
        input.get(); // Skip EOL character

        real len = 0;
        for (size_t a = 0; a < params.layer1_size; ++a)
            len += matrix[a + b * params.layer1_size] * matrix[a + b * params.layer1_size];
        len = sqrt(len);

        for (size_t a = 0; a < params.layer1_size; ++a)
            matrix[a + b * params.layer1_size] /= len;
    }
    input.close();

    vector<real> bestd;
    vector<string *> bestw;

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
        istringstream tokenize(line);

        while (not tokenize.eof())
        {
            string word;
            tokenize >> word;
            input_words.push_back(word);
        }

        size_t input_index = 0;
        vector<size_t> input_indexes;
        for (string const &word : input_words)
        {
            size_t vocab_index;
            for (vocab_index = 0; vocab_index < vocabulary.size(); ++vocab_index)
            {
                if (word == vocabulary[vocab_index])
                    break;
            }

            if (vocab_index == vocabulary.size())
                vocab_index = string::npos;

            if (vocab_index != string::npos)
            {
                cout << "\nWord: " << word << "  Position in vocabulary: " << vocab_index << endl;
                input_indexes.push_back(vocab_index);
            }
            else
                cout << "\nOut of dictionary word: " << word << endl;
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
        for (size_t vocab_index = 0; vocab_index < words; ++vocab_index)
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
                bestw.insert(bestw.begin() + cnt, &vocabulary[vocab_index]);
            }

            if (bestd.size() > NUM_WORDS)
            {
                bestd.erase(bestd.begin() + NUM_WORDS, bestd.end());
                bestw.erase(bestw.begin() + NUM_WORDS, bestw.end());
            }
        }

        for (size_t a = 0; a < bestd.size(); ++a)
            cout << (boost::format("%50s\t\t%f\n") % *bestw[a] % bestd[a]);
    }

    return 0;
}

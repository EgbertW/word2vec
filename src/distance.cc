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
typedef WordModel::WordResult WordResult;

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

    while (true)
    {
        printf("Enter word or sentence (EXIT to break): ");

        string line;
        getline(cin, line);

        if (line == "EXIT")
            break;

        vector<WordResult> results = model.findWords(line, NUM_WORDS);

        if (results.empty())
            continue;

        cout << "\n                                              Word       Cosine distance\n"
            "------------------------------------------------------------------------\n";

        for (WordResult const &r : results)
            cout << (boost::format("%50s\t\t%f\n") % r.second % r.first);
    }

    return 0;
}

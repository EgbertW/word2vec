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

#include <libword2vec/Vocabulary.h>
#include <fstream>
#include <iostream>
#include <boost/format.hpp>

using namespace std;
using namespace Word2Vec;

typedef Parameters::real real;

static void trainModel(Parameters &params, Vocabulary const &vocab, std::string const &output_file)
{
    cout << "Starting training using file " << params.train_file << endl;
    ifstream input(params.train_file, ios_base::in | ios_base::binary);
    ofstream output(output_file, ios_base::out | ios_base::binary);

    string last_word;
    size_t last_word_index = Vocabulary::npos;
    size_t cn = 0;

    size_t count_word_a = 0;
    size_t count_word_b = 0;
    size_t count_word_ab = 0;

    while (input.good())
    {
        string word;
        vocab.readWord(word, input);

        if (input.eof())
            break;

        
        if (word == "</s>")
        {
            output << "\n";
            continue;
        }

        ++cn;
        if ((params.debug_mode > 1) && (cn % 100000 == 0))
            cout << "Words written: " << (cn / 1000) << "K\r" << flush;

        bool out_of_voc = false;
        size_t word_index = vocab.search(word);
        if (word_index == Vocabulary::npos)
            out_of_voc = true;
        else
            count_word_b = vocab.get(word_index).cn();

        if (last_word_index == Vocabulary::npos)
            out_of_voc = true;

        last_word_index = word_index;
        string bigram_word = (boost::format("%s_%s") % last_word % word).str();

        if (bigram_word.length() > MAX_STRING)
            word_index = Vocabulary::npos;
        else
            word_index = vocab.search(bigram_word);

        if (word_index == Vocabulary::npos)
            out_of_voc = true;
        else
            count_word_ab = vocab.get(word_index).cn();

        if (count_word_a < params.min_count || count_word_b < params.min_count)
            out_of_voc = true;

        real score;
        if (out_of_voc) 
            score = 0;
        else
            score = (count_word_ab - params.min_count) / (real)count_word_a / (real)count_word_b * vocab.nTrainWords();

        if (score > params.phrase_threshold)
        {
            output << boost::format("_%s") % word;
            count_word_b = 0;
        }
        else
            output << boost::format(" %s") % word;

        count_word_a = count_word_b;
    }

    output.close();
    input.close();
}

static int findArgPos(char const *str, int argc, char **argv)
{
	int a;
	for (a = 1; a < argc; ++a)
    {
		if (!strcmp(str, argv[a]))
        {
			if (a == argc - 1)
            {
                printf("Argument missing for %s\n\n", str);
                return 1;
			}

            return a;
		}
    }

	return -1;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("WORD2PHRASE tool v0.1a\n\n");
        printf("Options:\n");
        printf("Parameters for training:\n");
        printf("\t-train <file>\n");
        printf("\t\tUse text data from <file> to train the model\n");
        printf("\t-output <file>\n");
        printf("\t\tUse <file> to save the resulting word vectors / word clusters / phrases\n");
        printf("\t-min-count <int>\n");
        printf("\t\tThis will discard words that appear less than <int> times; default is 5\n");
        printf("\t-threshold <float>\n");
        printf("\t\t The <float> value represents threshold for forming the phrases (higher means less phrases); default 100\n");
        printf("\t-debug <int>\n");
        printf("\t\tSet the debug mode (default = 2 = more info during training)\n");
        printf("\nExamples:\n");
        printf("./word2phrase -train text.txt -output phrases.txt -threshold 100 -debug 2\n\n");
        return 1;
    }

	string train_file;
    string output_file;

    Parameters params;
    params.train_phrase = true;
    params.alpha = make_shared<real>(0.025);
    params.word_count_actual = make_shared<size_t>(0);

    size_t arg_pos;
	if ((arg_pos = findArgPos("-train", argc, argv)) > 0) train_file = argv[arg_pos + 1];
	if ((arg_pos = findArgPos("-debug", argc, argv)) > 0) params.debug_mode = atoi(argv[arg_pos + 1]);
	if ((arg_pos = findArgPos("-output", argc, argv)) > 0) output_file = argv[arg_pos + 1];
	if ((arg_pos = findArgPos("-min-count", argc, argv)) > 0) params.min_count = atoi(argv[arg_pos + 1]);
    if ((arg_pos = findArgPos("-threshold", argc, argv)) > 0) params.phrase_threshold = atof(argv[arg_pos + 1]);

    params.train_file = train_file.c_str();

    shared_ptr<Vocabulary> vocab = make_shared<Vocabulary>(VOCAB_HASH_SIZE);

    vocab->readTrainFile(params);
    trainModel(params, *vocab, output_file);

    return 0;
}

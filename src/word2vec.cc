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


/**
* For more details see:
*
* http://arxiv.org/abs/1402.3722
* http://yinwenpeng.wordpress.com/2013/12/18/word2vec-gradient-calculation/
* http://yinwenpeng.wordpress.com/2013/09/26/hierarchical-softmax-in-neural-network-language-model/
*/

#include <libword2vec/Parameters.h>
#include <libword2vec/Vocabulary.h>
#include <libword2vec/WordModel.h>

#include <memory>

using namespace std;
using namespace Word2Vec;

static int syntax()
{
    printf("WORD VECTOR estimation toolkit v 0.1b\n\n");
    printf("Options:\n");
    printf("Parameters for training:\n");
    printf("\t-train <file>\n");
    printf("\t\tUse text data from <file> to train the model\n");
    printf("\t-output <file>\n");
    printf("\t\tUse <file> to save the resulting word vectors / word clusters\n");
    printf("\t-size <int>\n");
    printf("\t\tSet size of word vectors; default is 100\n");
    printf("\t-window <int>\n");
    printf("\t\tSet max skip length between words; default is 5\n");
    printf("\t-sample <float>\n");
    printf("\t\tSet threshold for occurrence of words. Those that appear with higher frequency");
    printf(" in the training data will be randomly down-sampled; default is 0 (off), useful value is 1e-5\n");
    printf("\t-hs <int>\n");
    printf("\t\tUse Hierarchical Softmax; default is 1 (0 = not used)\n");
    printf("\t-negative <int>\n");
    printf("\t\tNumber of negative examples; default is 0, common values are 5 - 10 (0 = not used)\n");
    printf("\t-threads <int>\n");
    printf("\t\tUse <int> threads (default 1)\n");
    printf("\t-min-count <int>\n");
    printf("\t\tThis will discard words that appear less than <int> times; default is 5\n");
    printf("\t-alpha <float>\n");
    printf("\t\tSet the starting learning rate; default is 0.025\n");
    printf("\t-classes <int>\n");
    printf("\t\tOutput word classes rather than word vectors; default number of classes is 0 (vectors are written)\n");
    printf("\t-debug <int>\n");
    printf("\t\tSet the debug mode (default = 2 = more info during training)\n");
    printf("\t-binary <int>\n");
    printf("\t\tSave the resulting vectors in binary moded; default is 0 (off)\n");
    printf("\t-save-vocab <file>\n");
    printf("\t\tThe vocabulary will be saved to <file>\n");
    printf("\t-read-vocab <file>\n");
    printf("\t\tThe vocabulary will be read from <file>, not constructed from the training data\n");
    printf("\t-cbow <int>\n");
    printf("\t\tUse the continuous bag of words model; default is 0 (skip-gram model)\n");

    printf("\nExamples:\n");
    printf("./word2vec -train data.txt -output vec.txt -debug 2 -size 200 -window 5 -sample 1e-4 -negative 5 -hs 0 -binary 0 -cbow 1\n\n");
    return 1;
}

static int ArgPos(char *str, int argc, char **argv)
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
        return syntax();

    Word2Vec::Parameters params;
    Word2Vec::Parameters::real alpha = 0.025;
    params.alpha = &alpha;

	string train_file;
	string save_vocab_file;
    string read_vocab_file;
    string output_file;

    size_t min_count;

    bool cbow = false;

    int arg_pos;
	if ((arg_pos = ArgPos((char *)"-size", argc, argv)) > 0) params.layer1_size = atoi(argv[arg_pos + 1]);
	if ((arg_pos = ArgPos((char *)"-train", argc, argv)) > 0) train_file = argv[arg_pos + 1];
	if ((arg_pos = ArgPos((char *)"-save-vocab", argc, argv)) > 0) save_vocab_file = argv[arg_pos + 1];
	if ((arg_pos = ArgPos((char *)"-read-vocab", argc, argv)) > 0) read_vocab_file = argv[arg_pos + 1];
	if ((arg_pos = ArgPos((char *)"-debug", argc, argv)) > 0) params.debug_mode = atoi(argv[arg_pos + 1]);
	if ((arg_pos = ArgPos((char *)"-binary", argc, argv)) > 0) params.binary = atoi(argv[arg_pos + 1]);
	if ((arg_pos = ArgPos((char *)"-cbow", argc, argv)) > 0) cbow = atoi(argv[arg_pos + 1]) != 0;
	if ((arg_pos = ArgPos((char *)"-alpha", argc, argv)) > 0) alpha = atof(argv[arg_pos + 1]);
	if ((arg_pos = ArgPos((char *)"-output", argc, argv)) > 0) output_file = argv[arg_pos + 1];
	if ((arg_pos = ArgPos((char *)"-window", argc, argv)) > 0) params.window = atoi(argv[arg_pos + 1]);
	if ((arg_pos = ArgPos((char *)"-sample", argc, argv)) > 0) params.sample = atof(argv[arg_pos + 1]);
	if ((arg_pos = ArgPos((char *)"-hs", argc, argv)) > 0) params.hs = atoi(argv[arg_pos + 1]) == 1;
	if ((arg_pos = ArgPos((char *)"-negative", argc, argv)) > 0) params.negative = atoi(argv[arg_pos + 1]);
	if ((arg_pos = ArgPos((char *)"-threads", argc, argv)) > 0) params.num_threads = atoi(argv[arg_pos + 1]);
	if ((arg_pos = ArgPos((char *)"-min-count", argc, argv)) > 0) min_count = atoi(argv[arg_pos + 1]);
	if ((arg_pos = ArgPos((char *)"-classes", argc, argv)) > 0) params.classes = atoi(argv[arg_pos + 1]);

    params.train_type = cbow ? Word2Vec::CBOW : Word2Vec::SKIP;
    params.output_file = output_file.c_str();
    params.train_file = train_file.c_str();
	
	/**
	Fixed starting Parameters:
	**/
	size_t vocab_hash_size =  30000000;  // Maximum 30 * 0.7 = 21M words in the vocabulary

	//1: init vocabulary
	shared_ptr<Vocabulary> vocab = make_shared<Vocabulary>(vocab_hash_size);

	//2: load vocab
	if (not read_vocab_file.empty())
		params.file_size = vocab->read(read_vocab_file, train_file, min_count);
	else
		params.file_size = vocab->readTrainFile(train_file, min_count);

	if (not save_vocab_file.empty())
		vocab->save(save_vocab_file);

	if (not output_file.empty()) //nowhere to output => quit
		return 0;


	//3: train_model
    params.vocabulary = vocab;
    WordModel model(params);
    model.train();

	return 0;
}

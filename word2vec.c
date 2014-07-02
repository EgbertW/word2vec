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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "vocab.h"

#define MAX_STRING 100
#define EXP_TABLE_SIZE 1000
#define MAX_EXP 6
#define MAX_SENTENCE_LENGTH 1000
#define MAX_CODE_LENGTH 40



typedef float real;                    // Precision of float numbers

char train_file[MAX_STRING], output_file[MAX_STRING];
char save_vocab_file[MAX_STRING], read_vocab_file[MAX_STRING];

int binary = 0, cbow = 0, debug_mode = 2, window = 5, min_count = 5, num_threads = 1, min_reduce = 1, ngram = 0, hashbang = 0, group_vec = 0;
long long layer1_size = 100;

long long word_count_actual = 0, file_size = 0, classes = 0;

real alpha = 0.025, starting_alpha, sample = 0;

//syn0 = vectors table
real *syn0, *syn1, *syn1neg, *expTable;

long long unsigned int saveArrayLength = 0;



clock_t start;

int hs = 1, negative = 0;

const int table_size = 1e8;

int *table;

struct threadParameters {
    vocabulary *voc;
    int threadNumber;
};

void InitUnigramTable(vocabulary * voc) {
	int a, i;
	long long train_words_pow = 0;
	real d1, power = 0.75;
	table = (int *)malloc(table_size * sizeof(int));


	for (a = 0; a < voc->vocab_size; a++)
		train_words_pow += pow(voc->vocab[a].cn, power); //occurences^power

	i = 0;
	d1 = pow(voc->vocab[i].cn, power) / (real)train_words_pow; //normalize

	for (a = 0; a < table_size; a++) {

		table[a] = i;

		if (a / (real)table_size > d1) {
		  i++;
		  d1 += pow(voc->vocab[i].cn, power) / (real)train_words_pow;
		}

		if (i >= voc->vocab_size)
			i = voc->vocab_size - 1;
		}
}

void DestroyNet() {
  if (syn0 != NULL) {
    free(syn0);
  }
  if (syn1 != NULL) {
    free(syn1);
  }
  if (syn1neg != NULL) {
    free(syn1neg);
  }
}

void InitNet(vocabulary * voc) {
	long long a, b;
	a = posix_memalign((void **)&syn0, 128, (long long)voc->vocab_size * layer1_size * sizeof(real));

	if (syn0 == NULL) {
		printf("Memory allocation failed\n"); 
		exit(1);
	}

	if (hs) {
		a = posix_memalign((void **)&syn1, 128, (long long)voc->vocab_size * layer1_size * sizeof(real));

		if (syn1 == NULL) {
			printf("Memory allocation failed\n");
			exit(1);
		}

		for (b = 0; b < layer1_size; b++)
			for (a = 0; a < voc->vocab_size; a++)
				 syn1[a * layer1_size + b] = 0;
	}

	if (negative>0) {
		a = posix_memalign((void **)&syn1neg, 128, (long long)voc->vocab_size * layer1_size * sizeof(real));

		if (syn1neg == NULL){
			printf("Memory allocation failed\n");
			exit(1);
		}

		for (b = 0; b < layer1_size; b++)
			for (a = 0; a < voc->vocab_size; a++)
		 		syn1neg[a * layer1_size + b] = 0;
	}

	for (b = 0; b < layer1_size; b++)
		for (a = 0; a < voc->vocab_size; a++)
			syn0[a * layer1_size + b] = (rand() / (real)RAND_MAX - 0.5) / layer1_size;

	CreateBinaryTree(voc);
}

void *TrainModelThread(void *arg) {

	struct threadParameters *params = arg;
	vocabulary *voc = params->voc;
	int id = params->threadNumber;

	long long a, b, d, i, word, last_word, sentence_length = 0, sentence_position = 0;
	long long word_count = 0, last_word_count = 0, sen[MAX_SENTENCE_LENGTH + 1];
	long long l1, l2, c, target, label;
	unsigned long long next_random = (long long)id;
	

	real f, g;
	clock_t now;

	char wordToGram[MAX_STRING];
	char gram[ngram+1];
	int start = 0;
	int end = ngram-1;
	int newWord = 1;
	int wordLength = 0;

	real *neu1 = (real *)calloc(layer1_size, sizeof(real)); //one vector
	real *neu1e = (real *)calloc(layer1_size, sizeof(real)); 
	FILE *fi = fopen(train_file, "rb");

	fseek(fi, file_size / (long long)num_threads * (long long)id, SEEK_SET);

	while (1) {


		if (word_count - last_word_count > 10000) {
			word_count_actual += word_count - last_word_count;
			last_word_count = word_count;

			if ((debug_mode > 1)) {
				now=clock();
				printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, alpha,
				word_count_actual / (real)(voc->train_words + 1) * 100,
				word_count_actual / ((real)(now - start + 1) / (real)CLOCKS_PER_SEC * 1000));
				fflush(stdout);
			}

			alpha = starting_alpha * (1 - word_count_actual / (real)(voc->train_words + 1));

			if (alpha < starting_alpha * 0.0001)
				alpha = starting_alpha * 0.0001;
		}

		if (sentence_length == 0) {

			while (1) {
				

				if (feof(fi))
					break;
				

				if(ngram > 0) //learn ngrams instead of words
				{
					
					
					if(newWord){
						ReadWord(wordToGram, fi,1);
						start = 0;
						end = ngram-1;
						wordLength = strlen(wordToGram);
					
						newWord = 0;
					}
					

					if(wordLength <= ngram){
						word =  SearchVocab(voc,wordToGram);
						newWord = 1;
						continue;
					}

					
					for (i = 0; i < ngram; i++)
					{
						gram[i] = wordToGram[start+i];
					}
					gram[ngram] = '\0';


					word = SearchVocab(voc, gram);
					
					end++;
					start++;

					if(end == wordLength)
						newWord = 1;
					
				}
				else
				{
				word = ReadWordIndex(voc,fi); 

				}
				

				if (word == -1)
					continue;

				word_count++;

				if (word == 0)
					break;

				// The subsampling randomly discards frequent words while keeping the ranking same
				if (sample > 0) {
					real ran = (sqrt(voc->vocab[word].cn / (sample * voc->train_words)) + 1) * (sample * voc->train_words) / voc->vocab[word].cn;
					next_random = next_random * (unsigned long long)25214903917 + 11;
					
					if (ran < (next_random & 0xFFFF) / (real)65536)
						continue;
				}
				sen[sentence_length] = word;
				sentence_length++;

				if (sentence_length >= MAX_SENTENCE_LENGTH)
					break;
			}
			
			sentence_position = 0;
		}

		if (feof(fi)) //end file
			break;

		if (word_count > voc->train_words / num_threads) //trained all word
			break;

		word = sen[sentence_position]; //index

		if (word == -1) 
			continue;

		for (c = 0; c < layer1_size; c++)
			neu1[c] = 0;

		for (c = 0; c < layer1_size; c++)
			neu1e[c] = 0;

		next_random = next_random * (unsigned long long)25214903917 + 11;

		b = next_random % window;


		if (cbow) {  //train the cbow architecture
			// in -> hidden
			for (a = b; a < window * 2 + 1 - b; a++) //a = [0 window]->[(window*2+1)-rand] -> dynamic window
				if (a != window) {

					c = sentence_position - window + a;
					
					if (c < 0) continue;

					if (c >= sentence_length) continue;

					last_word = sen[c]; //index of word

					if (last_word == -1) continue;

					for (c = 0; c < layer1_size; c++) // c is each vector index
						neu1[c] += syn0[c + last_word * layer1_size]; //sum of all vectors in input window (fig cbow) -> vectors on hidden
			}

			if (hs)
				for (d = 0; d < voc->vocab[word].codelen; d++) {
					f = 0;
					l2 = voc->vocab[word].point[d] * layer1_size; //offset of word
					// Propagate hidden -> output
					for (c = 0; c < layer1_size; c++)
						f += neu1[c] * syn1[c + l2]; //sum vectors input window * word weights on syn1 -> output vectors

					if (f <= -MAX_EXP) //sigmoid activation function - precalculated in expTable
						continue;
					else if (f >= MAX_EXP)
						continue;
					else
						f = expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))];

					// 'g' is the gradient multiplied by the learning rate
					g = (1 - voc->vocab[word].code[d] - f) * alpha; 
					// Propagate errors output -> hidden
					for (c = 0; c < layer1_size; c++)
						neu1e[c] += g * syn1[c + l2]; //save to modify vectors
					// Learn weights hidden -> output
					for (c = 0; c < layer1_size; c++)
						syn1[c + l2] += g * neu1[c]; //modify weights
				}
			// NEGATIVE SAMPLING
			if (negative > 0)
				for (d = 0; d < negative + 1; d++) {
					if (d == 0) {
						target = word;
						label = 1; //(w,c) in corpus
					} else {
						next_random = next_random * (unsigned long long)25214903917 + 11;
						target = table[(next_random >> 16) % table_size];

						if (target == 0) 
							target = next_random % (voc->vocab_size - 1) + 1;

						if (target == word)
							continue;

						label = 0; //(w,c) not in corpus
					}

					l2 = target * layer1_size; //get word vector index
					f = 0;

					for (c = 0; c < layer1_size; c++)
						f += neu1[c] * syn1neg[c + l2]; //vector*weights

					if (f > MAX_EXP) //sigmoid
						g = (label - 1) * alpha;
					else if (f < -MAX_EXP)
						g = (label - 0) * alpha;
					else
						g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * alpha;

					for (c = 0; c < layer1_size; c++)
						neu1e[c] += g * syn1neg[c + l2]; //saving error

					for (c = 0; c < layer1_size; c++)
						syn1neg[c + l2] += g * neu1[c];
				}
			// hidden -> in
			for (a = b; a < window * 2 + 1 - b; a++) if (a != window) {
				c = sentence_position - window + a;

				if (c < 0)
					continue;

				if (c >= sentence_length)
					continue;
				last_word = sen[c];

				if (last_word == -1)
					continue;

				for (c = 0; c < layer1_size; c++)
					syn0[c + last_word * layer1_size] += neu1e[c];  //modify word vectors with error
			}
		} else { 
				//SKIP-GRAM
			
			for (a = b; a < window * 2 + 1 - b; a++)
				if (a != window) {

					c = sentence_position - window + a;

					if (c < 0)
						continue;
					if (c >= sentence_length)
						continue;

					last_word = sen[c];

					if (last_word == -1)
						continue;

					l1 = last_word * layer1_size; //word index

					for (c = 0; c < layer1_size; c++)
						neu1e[c] = 0;

					// HIERARCHICAL SOFTMAX
					if (hs)
						for (d = 0; d < voc->vocab[word].codelen; d++) {
							f = 0;
							l2 = voc->vocab[word].point[d] * layer1_size; //other words
							// Propagate hidden -> output
							for (c = 0; c < layer1_size; c++)
								f += syn0[c + l1] * syn1[c + l2];

							if (f <= -MAX_EXP)
								continue;
							else if (f >= MAX_EXP)
								continue;
							else
								f = expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))];

							// 'g' is the gradient multiplied by the learning rate
							g = (1 - voc->vocab[word].code[d] - f) * alpha;
							// Propagate errors output -> hidden
							for (c = 0; c < layer1_size; c++)
								neu1e[c] += g * syn1[c + l2];
							// Learn weights hidden -> output
							for (c = 0; c < layer1_size; c++)
								syn1[c + l2] += g * syn0[c + l1];
						}
					// NEGATIVE SAMPLING
					if (negative > 0)
						for (d = 0; d < negative + 1; d++) {
							if (d == 0) {
								target = word;
								label = 1;
							} else {
								next_random = next_random * (unsigned long long)25214903917 + 11;
								target = table[(next_random >> 16) % table_size];

								if (target == 0)
									target = next_random % (voc->vocab_size - 1) + 1;

								if (target == word)
									continue;
								label = 0;
							}

						l2 = target * layer1_size;
						f = 0;

						for (c = 0; c < layer1_size; c++)
							f += syn0[c + l1] * syn1neg[c + l2];

						if (f > MAX_EXP)
							g = (label - 1) * alpha;
						else if (f < -MAX_EXP)
							g = (label - 0) * alpha;
						else
							g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * alpha;
						
						for (c = 0; c < layer1_size; c++)
							neu1e[c] += g * syn1neg[c + l2];
						
						for (c = 0; c < layer1_size; c++)
							syn1neg[c + l2] += g * syn0[c + l1];
					}
					// Learn weights input -> hidden
					for (c = 0; c < layer1_size; c++)
						syn0[c + l1] += neu1e[c];
			}
		}

		sentence_position++;

		if (sentence_position >= sentence_length) {
			sentence_length = 0;
			continue;
		}
	}

	fclose(fi);
	free(neu1);
	free(neu1e);
	pthread_exit(NULL);
}

void TrainModel(vocabulary* voc) {
	long a, b, c, d;
	FILE *fo;
	pthread_t *pt = (pthread_t *)malloc(num_threads * sizeof(pthread_t));

	starting_alpha = alpha;
	InitNet(voc);

	if (negative > 0)
		InitUnigramTable(voc);

	start = clock();

	struct threadParameters* params; 

	for (a = 0; a < num_threads; a++){
		params = malloc(sizeof(struct threadParameters));
		params->voc = voc;
		params->threadNumber = a;
		pthread_create(&pt[a], NULL, TrainModelThread, (void *)params);
	}

	for (a = 0; a < num_threads; a++)
		pthread_join(pt[a], NULL);

	if(debug_mode > 0)
		printf("Training Ended !\n");

	if(ngram > 0){
		free(pt);
		return;
	}

	fo = fopen(output_file, "wb");


	if (classes == 0 && ngram == 0) {
		// Save the word vectors
		fprintf(fo, "%lld %lld\n", voc->vocab_size, layer1_size);
		for (a = 0; a < voc->vocab_size; a++) {
			fprintf(fo, "%s ", voc->vocab[a].word);

			if (binary)
				for (b = 0; b < layer1_size; b++)
					fwrite(&syn0[a * layer1_size + b], sizeof(real), 1, fo);
			else
				for (b = 0; b < layer1_size; b++)
					fprintf(fo, "%lf ", syn0[a * layer1_size + b]);

			fprintf(fo, "\n");
		}
	} else {
		// Run K-means on the word vectors
		int clcn = classes, iter = 10, closeid;
		int *centcn = (int *)malloc(classes * sizeof(int));
		int *cl = (int *)calloc(voc->vocab_size, sizeof(int));
		real closev, x;
		real *cent = (real *)calloc(classes * layer1_size, sizeof(real));

		for (a = 0; a < voc->vocab_size; a++)
			cl[a] = a % clcn;

		for (a = 0; a < iter; a++) {
			for (b = 0; b < clcn * layer1_size; b++)
				cent[b] = 0;

			for (b = 0; b < clcn; b++)
				centcn[b] = 1;

			for (c = 0; c < voc->vocab_size; c++) {

				for (d = 0; d < layer1_size; d++)
					cent[layer1_size * cl[c] + d] += syn0[c * layer1_size + d];

				centcn[cl[c]]++;
			}

			for (b = 0; b < clcn; b++) {
				closev = 0;

				for (c = 0; c < layer1_size; c++) {
					cent[layer1_size * b + c] /= centcn[b];
					closev += cent[layer1_size * b + c] * cent[layer1_size * b + c];
				}

				closev = sqrt(closev);
				for (c = 0; c < layer1_size; c++)
					cent[layer1_size * b + c] /= closev;
			}

			for (c = 0; c < voc->vocab_size; c++) {
				closev = -10;
				closeid = 0;
				for (d = 0; d < clcn; d++) {
					x = 0;
					for (b = 0; b < layer1_size; b++)
						x += cent[layer1_size * d + b] * syn0[c * layer1_size + b];

					if (x > closev) {
						closev = x;
						closeid = d;
					}
				}
				cl[c] = closeid;
			}
		}
		// Save the K-means classes

		for (a = 0; a < voc->vocab_size; a++){
			fprintf(fo, "%s %d", voc->vocab[a].word, cl[a]);

			for (b = 0; b < layer1_size; b++){
				fprintf(fo, "%lf ", syn0[a * layer1_size + b]);
			}
			fprintf(fo, "\n");
		}

		free(centcn);
		free(cent);
		free(cl);
		
	}

	fclose(fo);
	free(pt);

}

/*group by sum*/
void sumGram(int offset, real* vector)
{
	int i;
	for (i=0; i < layer1_size;i++)
	{
		vector[i]+=syn0[offset+i];
	}
}

/*group by sum*/
void sumFreqGram(int offset, real* vector,int indGram)
{
	//Useless function?
	sumGram(offset,vector);
	/*
	int i;
	for (i=0; i < layer1_size;i++)
	{
		vector[i]+=(syn0[offset+i]* (1.00 / vocab[indGram].cn));
	}
	*/
}

/*1->min 0->max*/
void minmaxGram(int offset,real *vector,int min)
{
	int i;

	if(min){
		for (i=0; i < layer1_size;i++)
		{
			if(vector[i]>syn0[offset+i])
				vector[i]=syn0[offset+i];
		}
	}
	else
	{
		for (i=0; i < layer1_size;i++)
		{
			if(vector[i]<syn0[offset+i])
				vector[i]=syn0[offset+i];
		}
	}
}

/*Divide vector by ngrams*/
void truncGram(int offset, real *vector, int wordLength, int gramPos)
{
	//nbGram = wordLength - ngram + 1;
	int nbDiv = layer1_size/(wordLength - ngram + 1);
	int i;
	// if nbdiv*nbGram =! layer1size
	for(i=gramPos*nbDiv;i<layer1_size;i++)//overrides - make condition cleaner 
	{ 
		vector[i] = syn0[offset+i];
	}
}

void createWordVector(vocabulary* voc,char* train_file, char* output_file){
	FILE *fin, *fo;
	char grama[ngram+1];
	int hash = 0;
	char word[MAX_STRING];
	int i,start,end,lenWord,indGram, offset;
	int *hashset;
	long long unsigned int cptWord=0;
	int skipCpt=0;
	int unexistCpt=0;
	int gramCpt=0;

	hashset = calloc(voc->vocab_hash_size,sizeof(int));
	real wordVec[layer1_size];

	for(i=0;i<voc->vocab_hash_size;i++)
		hashset[i] = -1;

	fin = fopen(train_file, "rb");
	fo = fopen(output_file, "wb");

	if (fin == NULL) {
		printf("ERROR: training data file not found!\n");
		exit(1);
	}

	/* Counting number of words in file*/
	while (1) {
		
		if (feof(fin))
			break;

		ReadWord(word, fin,hashbang);
		hash = GetWordHash(voc,word);

		if (hashset[hash] != -1)
			continue;
		else
			hashset[hash] = 1;

		cptWord++;
	}

	
	
	
	
	fprintf(fo, "%lld %lld\n", cptWord+1, layer1_size); //prints size + 1 for </s>


	if(debug_mode > 0)
		printf("number of words: %lld\n",cptWord+1 );

	/*reset*/
	rewind(fin);
	for(i=0;i<voc->vocab_hash_size;i++)
		hashset[i] = -1;

	for(i=0;i<layer1_size;i++) 
		wordVec[i] = 0;

	cptWord=0;

	
	/*write </s>*/

	/*
	indGram = SearchVocab("</s>");
	offset = indGram * layer1_size;
	*/

	for(i=0;i<layer1_size;i++)
	{
		wordVec[i] = 0; //syn0[offset+i];
	}
	
	fprintf(fo, "</s> ");
	for (i = 0; i < layer1_size; i++){
		if (binary)
			fwrite(&wordVec[i], sizeof(real), 1, fo);
		else
			fprintf(fo, "%lf ", wordVec[i]);
	}
	fprintf(fo, "\n");
	
	cptWord=1;

	/*Writing vectors*/
	while (1) {
		
		if (feof(fin))
			break;

		ReadWord(word, fin,hashbang);

		hash = GetWordHash(voc,word);

		for(i=0;i<layer1_size;i++) 
			wordVec[i] = 0;

		lenWord = strlen(word);
		start = 0;
		end = ngram-1;

		if(hashset[hash] != -1){
			skipCpt++;
			continue;
		}

		while(end<lenWord)
		{

			for (i = 0; i < ngram; i++)
			{
				grama[i] = word[start+i];
			}

			grama[ngram] = '\0';
			indGram = SearchVocab(voc,grama);

			if(indGram > -1)
				offset = indGram * layer1_size;
			else
			{
				unexistCpt++;
				end++;
				start++;
				continue;
			}
			switch(group_vec){
				case 0:
				case 1:
					sumGram(offset,wordVec);
					break;
				case 2:
					minmaxGram(offset,wordVec,1);
					break;
				case 3:
					minmaxGram(offset,wordVec,0);
					break;
				case 4:
					truncGram(offset,wordVec,lenWord,gramCpt);
					break;
				case 5:
					sumFreqGram(offset,wordVec,indGram);
			}

			gramCpt++;
			end++;
			start++;
		}

		if(group_vec==0 || group_vec==5) //Mean
		{
			//normalization
			for(i=0;i<layer1_size;i++){
					wordVec[i] /= gramCpt;
			}
		}
		hashset[hash] = 1;		
		gramCpt = 0;

		//removes #bangs
		if(hashbang > 0){
			for(i=1;i<lenWord;i++){
				word[i-1]=word[i];
			}
			word[lenWord-2]='\0';
		}

		
		fprintf(fo, "%s ", word);

		for (i = 0; i < layer1_size; i++){
			if (binary)
					fwrite(&wordVec[i], sizeof(real), 1, fo);
			else
					fprintf(fo, "%lf ", wordVec[i]);
		}

		fprintf(fo, "\n");
		
		
		
		cptWord++;
		
	}

	if(debug_mode > 0)
		printf("Saved %lld word vectors, %d grams weren't in dictionnary, %d words were skipped (doubles)\n",cptWord,unexistCpt,skipCpt);

	fclose(fo);
	fclose(fin);
	free(hashset);
}

/* in case of double training
void mergeAndSaveVectors(){

	int i = 0;
	int j = 0;
	int indWord;
	int offset;
	char * word;
	real wordVec[layer1_size];
	int cptSkipped = 0;
	FILE *fo;

	fo = fopen(output_file, "wb");
	fprintf(fo, "%lld %lld\n", saveArrayLength, layer1_size); //prints size

	for(i=0;i<saveArrayLength;i++){
		word = saveArray[i].word;

		for (j = 0; j < layer1_size; j++)
		{
			wordVec[j] = saveArray[i].vector[j];
		}
		
		indWord = SearchVocab(word);
		
		if(indWord == -1)
		{
			cptSkipped++;
		}
		else
		{
			offset = indWord*layer1_size;

			for (j = 0; j < layer1_size; j++)
			{
				wordVec[j] = (wordVec[j] + syn0[offset+j])/2; //mean
			}
		}
		
		//save to file

		fprintf(fo, "%s ", word);

			for (j = 0; j < layer1_size; j++){
				if (binary)
						fwrite(&wordVec[j], sizeof(real), 1, fo);
				else
						fprintf(fo, "%lf ", wordVec[j]);
			}

		fprintf(fo, "\n");
	}
	printf("skipped %d/%lld words, they were down-sampled by word training - they only have syntactic vectors \n",cptSkipped,saveArrayLength );
}
*/

int ArgPos(char *str, int argc, char **argv) {
	int a;
	for (a = 1; a < argc; a++)
		if (!strcmp(str, argv[a])) {

			if (a == argc - 1) {
			printf("Argument missing for %s\n", str);
			exit(1);
			}

		return a;
		}

	return -1;
}

int main(int argc, char **argv) {
	int i;
	if (argc == 1) {
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
		printf("\t-ngram <int> (default 0 - use words) \n");
		printf("\t\tUse N-GRAM model instead of words to train vectors \n");
		printf("\t-hashbang <0-1> (default 0)\n");
		printf("\t\tUse hashbang on n-grams - i.e #good# -> #go,goo,ood,od#\n");
		printf("\t-group <0-5> (default 0)\n");
		printf("\t\tHow word vectors are computed with n-grams - 0:Mean (default); 1:Sum; 2:Min; 3:Max; 4:Trunc; 5:FreqSum\n");
		
		printf("\nExamples:\n");
		printf("./word2vec -train data.txt -output vec.txt -debug 2 -size 200 -window 5 -sample 1e-4 -negative 5 -hs 0 -binary 0 -cbow 1\n\n");
		return 0;
	}

	output_file[0] = 0;
	save_vocab_file[0] = 0;
	read_vocab_file[0] = 0;

	if ((i = ArgPos((char *)"-size", argc, argv)) > 0)layer1_size = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-train", argc, argv)) > 0) strcpy(train_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-save-vocab", argc, argv)) > 0) strcpy(save_vocab_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-read-vocab", argc, argv)) > 0) strcpy(read_vocab_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-debug", argc, argv)) > 0) debug_mode = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-binary", argc, argv)) > 0) binary = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-cbow", argc, argv)) > 0) cbow = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-alpha", argc, argv)) > 0) alpha = atof(argv[i + 1]);
	if ((i = ArgPos((char *)"-output", argc, argv)) > 0) strcpy(output_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-window", argc, argv)) > 0) window = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-sample", argc, argv)) > 0) sample = atof(argv[i + 1]);
	if ((i = ArgPos((char *)"-hs", argc, argv)) > 0) hs = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-negative", argc, argv)) > 0) negative = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-threads", argc, argv)) > 0) num_threads = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-min-count", argc, argv)) > 0) min_count = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-classes", argc, argv)) > 0) classes = atoi(argv[i + 1]);
	if ((i = ArgPos ((char *) "-ngram", argc, argv)) > 0 ) ngram = atoi(argv[i + 1]);
	if ((i = ArgPos ((char *) "-hashbang", argc, argv)) > 0 ) hashbang = atoi(argv[i + 1]);
	if ((i = ArgPos ((char *) "-group", argc, argv)) > 0 ) group_vec = atoi(argv[i + 1]);
	
	expTable = (real *)malloc((EXP_TABLE_SIZE + 1) * sizeof(real));

	for (i = 0; i < EXP_TABLE_SIZE; i++) {
		expTable[i] = exp((i / (real)EXP_TABLE_SIZE * 2 - 1) * MAX_EXP); // Precompute the exp() table
		expTable[i] = expTable[i] / (expTable[i] + 1);                   // Precompute f(x) = x / (x + 1)
	}

	/**
	Fixed starting Parameters:
	**/
	const long long vocab_hash_size =  30000000;  // Maximum 30 * 0.7 = 21M words in the vocabulary
	const int vocab_max_size = 1000;

	//1: init vocabulary
	vocabulary* vocab = InitVocabulary(vocab_hash_size,vocab_max_size);

	//2: load vocab
	if (read_vocab_file[0] != 0)
		ReadVocab(vocab,read_vocab_file,train_file);
	else
		LearnVocabFromTrainFile(vocab,train_file,min_count,ngram,hashbang);

	if (save_vocab_file[0] != 0)
		SaveVocab(vocab,save_vocab_file);

	if (output_file[0] == 0) //nowhere to output => quit
		return 0;

	//3: train_model
	TrainModel(vocab);
	
	if(ngram > 0){
		printf("Creating word vectors.\n");
		createWordVector(vocab,train_file,output_file);
	}


	free(expTable);
	DestroyNet();
	DestroyVocab(vocab);

	return 0;
}
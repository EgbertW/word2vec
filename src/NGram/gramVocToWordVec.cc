
void gramVocToWordVec(vocabulary* voc, real* syn0,int max_string, int layer1_size, int ngram, int hashbang,int group_vec, int binary,int position,int overlap, char* train_file, char* output_file){

	FILE *fin, *fo;
	char grama[ngram+3];
	int hash = 0;
	char word[max_string];
	int i,start=0,end=0,lenWord, offset;
	int *hashset;
	long long unsigned int cptWord=0;
	long long int indGram;
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

		ReadWord(word, fin);
		hash = GetWordHash(voc,word);

		if (hashset[hash] != -1)
			continue;
		else
			hashset[hash] = 1;

		cptWord++;
	}
	
	fprintf(fo, "%lld %d\n", cptWord+1, layer1_size); //prints size + 1 for </s>

	if(DEBUG_MODE > 0)
		printf("number of words: %lld\n",cptWord+1 );

	/*reset*/
	rewind(fin);
	for(i=0;i<voc->vocab_hash_size;i++)
		hashset[i] = -1;

	for(i=0;i<layer1_size;i++) 
		wordVec[i] = 0;

	cptWord=0;

	
	/*write </s>*/

	for(i=0;i<layer1_size;i++)
	{
		wordVec[i] = syn0[i]; //"</s> is always 0"
	}
	
	fprintf(fo, "</s> ");
	for (i = 0; i < layer1_size; i++){
		if (binary)
			fwrite(&wordVec[i], sizeof(real), 1, fo);
		else
			fprintf(fo, "%lf ", wordVec[i]);
	}
	fprintf(fo, "\n");

	hash = GetWordHash(voc,"</s>");
	hashset[hash] = 1;
	
	cptWord=1;

	/*Writing vectors*/

	while (1) {
		
		if (feof(fin))
			break;

		if (hashbang)
			ReadWordHashbang(word, fin);
		else
			ReadWord(word,fin);

		hash = GetWordHash(voc,word);

		for(i=0;i<layer1_size;i++) 
			wordVec[i] = 0;

		lenWord = strlen(word);

		if(hashset[hash] != -1){
			skipCpt++;
			continue;
		}


		if(lenWord > ngram){
			while(getGrams(word,grama,gramCpt, ngram, overlap, position,hashbang))
			{
				
				indGram = SearchVocab(voc,grama);
				//printf("word:%s, gram:%s, ind:%lld\n",word,grama,indGram );
				if(indGram > -1)
					offset = indGram * layer1_size;
				else
				{
					unexistCpt++;
					gramCpt += 1;
					continue;
				}
				
				switch(group_vec){
					case 0:
					case 1:
						sumGram(syn0, layer1_size, offset,wordVec);
						break;
					case 2:
						minmaxGram(syn0, layer1_size,offset,wordVec,1);
						break;
					case 3:
						minmaxGram(syn0, layer1_size,offset,wordVec,0);
						break;
					case 4:
						truncGram(syn0, layer1_size,ngram,offset,wordVec,lenWord,gramCpt);
						break;
					case 5:
						sumFreqGram(syn0, layer1_size,offset,wordVec,voc->vocab[indGram].cn);
				}

				gramCpt++;
				end++;
				start++;
			}

			//normalization
			for(i=0;i<layer1_size;i++){
					wordVec[i] /= gramCpt;
			}
			

		}
		else
		{
			indGram = SearchVocab(voc,word);

			if(indGram > -1){

				offset = indGram * layer1_size;
				for (i=0; i < layer1_size;i++)
				{
					wordVec[i]+=syn0[offset+i];
				}

			}
			else{
				unexistCpt++;
			}
		}


		hashset[hash] = 1;		
		gramCpt = 0;

		//removes #bangs
		if(hashbang > 0)
		{
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

	if(DEBUG_MODE > 0)
		printf("Saved %lld word vectors, %d grams weren't in dictionnary, %d words were skipped (doubles)\n",cptWord,unexistCpt,skipCpt);

	fclose(fo);
	fclose(fin);
	free(hashset);
}

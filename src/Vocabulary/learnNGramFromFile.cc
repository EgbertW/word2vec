
/*Create a vocab of ngram from train file*/
long long LearnNGramFromTrainFile(vocabulary* voc, char* train_file,int min_count, int ngram, int hashbang, int position, int overlap) {
	char word[MAX_STRING];
	int i,lenWord;
	FILE * fin;
	
	char gram[ngram*2+4]; //possibility to merge a ngram with another one < ngram size + position (3 tokens) + '\0'

	for (i = 0; i < voc->vocab_hash_size; i++) //init vocab hashtable
		voc->vocab_hash[i] = -1;

	fin = fopen(train_file, "rb");

	if (fin == NULL) {
		printf("ERROR: training data file not found!\n");
		exit(1);
	}
	
	voc->vocab_size = 0;
	AddWordToVocab(voc, (char *)"</s>");

	while (1) {

		if(hashbang)
			ReadWordHashbang(word,fin);
		else
			ReadWord(word,fin);

		lenWord = strlen(word);

		if(lenWord<=ngram){ //word smaller or equal to ngram var.
			searchAndAddToVocab(voc,word);

			if (feof(fin))
				break;
			else
				continue;
		}


		i=0;
		while(getGrams(word,gram,i, ngram, overlap, position,hashbang)){
			searchAndAddToVocab(voc,gram);
			i++;
		}

		if (feof(fin))
			break;

		voc->train_words++;

		if ((DEBUG_MODE > 1) && (voc->train_words % 100000 == 0)) {
			printf("%lldK%c", voc->train_words / 1000, 13);
			fflush(stdout);
		}
	}
	printf("Vocab size: %lld  - min_count %d \n", voc->vocab_size,min_count);
	SortVocab(voc,min_count);

	if (DEBUG_MODE > 1) {
		printf("Vocab size: %lld\n", voc->vocab_size);
		printf("Words in train file: %lld\n", voc->train_words);
	}

	long long file_size = ftell(fin);
	fclose(fin);
	return file_size;
}


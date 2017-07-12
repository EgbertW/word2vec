
/*Create a vocab from train file*/
long long LearnVocabFromTrainFile(vocabulary* voc, char* train_file,int min_count) {
	int i;
	char word[MAX_STRING];
	FILE * fin;

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

		ReadWord(word, fin);
		searchAndAddToVocab(voc,word);
		
		if (feof(fin))
			break;

		voc->train_words++;

		if ((DEBUG_MODE > 1) && (voc->train_words % 100000 == 0)) {
			printf("%lldK%c", voc->train_words / 1000, 13);
			fflush(stdout);
		}
	}

	SortVocab(voc,min_count);

	if (DEBUG_MODE > 1) {
		printf("Vocab size: %lld\n", voc->vocab_size);
		printf("Words in train file: %lld\n", voc->train_words);
	}

	long long file_size = ftell(fin);
	fclose(fin);
	return file_size;
}


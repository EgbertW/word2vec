
/*Reads a saved vocab file*/
long long ReadVocab(vocabulary* voc, char* read_vocab_file, char* train_file,int min_count) {
	long long a, i = 0;
	char c;
	char word[MAX_STRING];
	FILE *fin = fopen(read_vocab_file, "rb");

	if (fin == NULL) {
		printf("Vocabulary file not found\n");
		exit(1);
	}

	for (a = 0; a < voc->vocab_hash_size; a++)
		voc->vocab_hash[a] = -1;

	voc->vocab_size = 0;

	while (1) {
		ReadWord(word, fin);

		if (feof(fin))
			break;

		a = AddWordToVocab(voc,word);
		fscanf(fin, "%lld%c", &voc->vocab[a].cn, &c);
		i++;
	}

	SortVocab(voc,min_count);

	if (DEBUG_MODE > 1) {
		printf("Vocab size: %lld\n", voc->vocab_size);
		printf("Words in train file: %lld\n", voc->train_words);
	}

	fin = fopen(train_file, "rb");

	if (fin == NULL) {
		printf("ERROR: training data file not found!\n");
		exit(1);
	}

	fseek(fin, 0, SEEK_END);
	long long file_size = ftell(fin);
	fclose(fin);
	return file_size;
}


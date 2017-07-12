
/*Inits a vocabulary*/
vocabulary* InitVocabulary(int vocab_hash_size, int vocab_max_size){
	int i;

	vocabulary* voc = (vocabulary*) malloc(sizeof(vocabulary));
	if(voc == NULL){
		printf("vocabulary couldn't be created, memory problem, exiting...\n");
		exit(1);
	}
	voc->vocab_hash = (int *) calloc(vocab_hash_size, sizeof(int));
	if(voc == NULL){
		printf("vocabulary hash couldn't be created, memory problem, exiting...\n");
		exit(1);
	}

	voc->vocab = (struct vocab_word *) calloc(vocab_max_size, sizeof(struct vocab_word));
	if(voc == NULL){
		printf("vocabulary hash couldn't be created, memory problem, exiting...\n");
	}
	voc->vocab_size = 0;
	voc->vocab_hash_size = vocab_hash_size;
	voc->vocab_max_size = vocab_max_size;
	voc->train_words = 0;

	for(i=0;i<voc->vocab_hash_size;i++)
		voc->vocab_hash[i] = -1;

	for(i=0;i<voc->vocab_max_size;i++)
		voc->vocab[i].cn = 0;

	return voc;
}




/* Adds a word to the vocabulary
	Returns the vocabulary size */
int AddWordToVocab(vocabulary* voc, char *word) {
	unsigned int hash, length = strlen(word) + 1;

	if (length > MAX_STRING)
		length = MAX_STRING;

	
	voc->vocab[voc->vocab_size].word = (char *)calloc(length, sizeof(char));
	strcpy(voc->vocab[voc->vocab_size].word, word);

	voc->vocab[voc->vocab_size].cn = 0;
	voc->vocab_size++;

	// Reallocate memory if needed
	if (voc->vocab_size + 2 >= voc->vocab_max_size) {
		voc->vocab_max_size += 1000;
		voc->vocab = (struct vocab_word *)realloc(voc->vocab, voc->vocab_max_size * sizeof(struct vocab_word));
	}
	
	hash = GetWordHash(voc,word);

	while (voc->vocab_hash[hash] != -1)
		hash = (hash + 1) % voc->vocab_hash_size;

	voc->vocab_hash[hash] = voc->vocab_size - 1;
	return voc->vocab_size - 1;
}


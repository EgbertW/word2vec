
/* Returns position of a word in the vocabulary;
 if the word is not found, returns -1*/
int SearchVocab(vocabulary* voc, char *word) {
	int hash = GetWordHash(voc, word);
	
	while (1) {
		if (voc->vocab_hash[hash] == -1)
			return -1;

		if (!strcmp(word, voc->vocab[voc->vocab_hash[hash]].word))
			return voc->vocab_hash[hash];

		hash = (hash + 1) % voc->vocab_hash_size;
	}

	return -1;
}



/* Returns hash value of a word*/
int GetWordHash(vocabulary* voc, char *word) {
	unsigned long long a, hash = 0;

	for (a = 0; a < strlen(word); a++){ 
		hash = hash * 257 + word[a];
	}

	hash = hash % voc->vocab_hash_size;
	return hash;
}


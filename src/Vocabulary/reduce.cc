
/* Reduces the vocabulary by removing infrequent tokens */
void ReduceVocab(vocabulary* voc, int min_reduce) {

	int a, b = 0;
	unsigned int hash;

	for (a = 0; a < voc->vocab_size; a++){
		if (voc->vocab[a].cn > min_reduce) {

			voc->vocab[b].cn = voc->vocab[a].cn;
			voc->vocab[b].word = voc->vocab[a].word;
			b++;

		} else
			free(voc->vocab[a].word);
	}

	voc->vocab_size = b;

	for (a = 0; a < voc->vocab_hash_size; a++)
		voc->vocab_hash[a] = -1;

	for (a = 0; a < voc->vocab_size; a++) {
		// Hash will be re-computed, as it is not actual
		hash = GetWordHash(voc, voc->vocab[a].word);

		while (voc->vocab_hash[hash] != -1)
			hash = (hash + 1) % voc->vocab_hash_size;

		voc->vocab_hash[hash] = a;
	}

	fflush(stdout);
	min_reduce++;
}



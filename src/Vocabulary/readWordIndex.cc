
/* Reads a word and returns its index in the vocabulary*/
int ReadWordIndex(vocabulary* voc, FILE *fin) {
	char word[MAX_STRING];
	ReadWord(word, fin);

	if (feof(fin)) 
		return -1;

	return SearchVocab(voc, word);
}


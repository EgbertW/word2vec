
/*Look if word already in vocab, if not add, if yes, increment. */
void searchAndAddToVocab(vocabulary* voc, char* word){
	//printf("searching for \"%s\" in vocab\n",word );
	int a,i;
	i = SearchVocab(voc, word);

		if (i == -1) {
			//printf("wasn't there, adding \"%s\" to vocab\n",word );
			a = AddWordToVocab(voc, word);
			voc->vocab[a].cn = 1;
		} else
			//printf("was there, increment its cn value \"%s\" => %lld to vocab\n",word,voc->vocab[i].cn+1);
			voc->vocab[i].cn++;

		if (voc->vocab_size > voc->vocab_hash_size * 0.7)
			ReduceVocab(voc,1);  //////////////////CAUTION FIXED PARAMETER


		i = SearchVocab(voc, word);
		if(i== -1){
			//printf("%s wasn't properly imported, dafuq? \n", word);
		} 
}


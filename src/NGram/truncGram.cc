
/*Divide vector by ngrams*/
void truncGram(real* syn0, int layer1_size,int ngram,int offset, real *vector, int wordLength, int gramPos)
{
	//nbGram = wordLength - ngram + 1;
	int nbDiv = layer1_size/(wordLength - ngram + 1);
	int i;
	// if nbdiv*nbGram =! layer1size
	for(i=gramPos*nbDiv;i<layer1_size;i++)//overrides - make condition cleaner 
	{ 
		vector[i] = syn0[offset+i];
	}
}


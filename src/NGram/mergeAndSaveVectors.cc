
void mergeAndSaveVectors()
{
	int i = 0;
	int j = 0;
	int indWord;
	int offset;
	char * word;
	real wordVec[layer1_size];
	int cptSkipped = 0;
	FILE *fo;

	fo = fopen(output_file, "wb");
	fprintf(fo, "%lld %lld\n", saveArrayLength, layer1_size); //prints size

	for(i=0;i<saveArrayLength;i++){
		word = saveArray[i].word;

		for (j = 0; j < layer1_size; j++)
		{
			wordVec[j] = saveArray[i].vector[j];
		}
		
		indWord = SearchVocab(word);
		
		if(indWord == -1)
		{
			cptSkipped++;
		}
		else
		{
			offset = indWord*layer1_size;

			for (j = 0; j < layer1_size; j++)
			{
				wordVec[j] = (wordVec[j] + syn0[offset+j])/2; //mean
			}
		}
		
		//save to file

		fprintf(fo, "%s ", word);

			for (j = 0; j < layer1_size; j++){
				if (binary)
						fwrite(&wordVec[j], sizeof(real), 1, fo);
				else
						fprintf(fo, "%lf ", wordVec[j]);
			}

		fprintf(fo, "\n");
	}
	printf("skipped %d/%lld words, they were down-sampled by word training - they only have syntactic vectors \n",cptSkipped,saveArrayLength );
}


int getGrams(char* word, char* gram, int index, int size,int overlap,int position,int hashbang){
	int lenWord = strlen(word);
	int lastIndex;

	if(overlap)
		lastIndex = lenWord-size;
	else
		lastIndex = lenWord/size-1;

	if(index > lastIndex)
		return 0;

	if(lenWord <= size){
		return -1; //usefull for training threads
	}

	int start,i;

	for(i = 0;i<size*2+4;i++) //gram = ngram*2+4 (convention - see vocab.c -> LearnNGramFromTrainFile)
		gram[i]=0; //reset gram

	if(overlap){
		start = index;
		
		for (i = 0; i < size; i++)
		{
			gram[i] = word[start+i];
		}
		gram[size] = '\0';

		if(position > 0)
			addGramPosition(word,gram,size,index,position,overlap,hashbang);
	}
	else
	{
		
		start = index * size;
	
		i=0;

		if(index == lastIndex)
		{
			while(word[start+i] != '\0'){
				gram[i] = word[start+i];
				i++;
			}
			gram[i] = '\0';
		}
		else 
		{
			for(i=0;i<size;i++)
				gram[i]=word[start+i];
			gram[size]='\0';
		}


		if(position > 0)
			addGramPosition(word,gram,size,index,position,overlap, hashbang);
	}
	

	return 1;
}


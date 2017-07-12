
/* Adds position to gram Ngram - gram tab size is ngram+3 index: [0->ngram+2]*/
void addGramPosition(char* word, char * gram, int size, int index, int position, int overlap,int hashbang){
	int i;
	char num[3];
	int lenWord = strlen(word);
	int lenGram = strlen(gram);
	int lastIndex;

	if(overlap)
		lastIndex = lenWord-size;
	else
		lastIndex = lenWord/size-1;
	
	if(lastIndex == 0)
		return; 

	if(position==1){
		/*	Adds '-' /!\ intended for no hashbangs */


		if(index==0 ) //first index
		{
			gram[size]='-';
			gram[size+1]='\0'; 
			return;
		}

		if(index == lastIndex) //last index
		{
			for(i=lenGram+1;i>0;i--){
				gram[i]=gram[i-1];
			}
			gram[0]='-';

			return;
		}

		for(i=lenGram+1;i>0;i--){
			gram[i]=gram[i-1];
		}
			gram[0]='-';
			gram[size+1]='-';
			gram[size+2]='\0';
	}
	else
	{
		/* adds #index- /!\ start must be <= 99 */

		if(index==0 && gram[0]=='#')
			return;

		if(index == lastIndex && hashbang)
			return;

		for(i= lenGram+1;i>=0;i--)
		{
			gram[i+3]=gram[i];
		}
		
		sprintf(num,"%d",index);
		if(index>=10){
			gram[0] = num[0];	
			gram[1] = num[1];
		}else{
			gram[0] = '0';
			gram[1] = num[0];
		}
		gram[2] = '-';

	}
	
	return;
}


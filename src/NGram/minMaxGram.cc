
/*1->min 0->max*/
void minMaxGram(real* syn0, int layer1_size,int offset,real *vector,int min)
{
	int i;

	if(min){
		for (i=0; i < layer1_size;i++)
		{
			if(vector[i]>syn0[offset+i])
				vector[i]=syn0[offset+i];
		}
	}
	else
	{
		for (i=0; i < layer1_size;i++)
		{
			if(vector[i]<syn0[offset+i])
				vector[i]=syn0[offset+i];
		}
	}
}


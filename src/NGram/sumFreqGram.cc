
/*group by sum*/
void sumFreqGram(real* syn0, int layer1_size,int offset, real* vector,int cn)
{
	int i;
	for (i=0; i < layer1_size;i++)
	{
		vector[i]+=(syn0[offset+i]* (1.00 / cn));
	}

}



/*group by sum*/
void sumGram(real* syn0, int layer1_size, int offset, real* vector)
{
	int i;
	for (i=0; i < layer1_size;i++)
	{
		vector[i]+=syn0[offset+i];
	}
}


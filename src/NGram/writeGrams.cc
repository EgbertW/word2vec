
void writeGrams(vocabulary* voc,real *syn0,int layer1_size,int ngram,int hashbang,int position,char* output_file, int binary){
	FILE *fo = fopen(output_file,"wb");
	int a,b;


	fprintf(fo, "%lld %d %d %d %d\n", voc->vocab_size, layer1_size, ngram, hashbang, position);
	for (a = 0; a < voc->vocab_size; a++) {
		fprintf(fo, "%s ", voc->vocab[a].word);

		if (binary)
			for (b = 0; b < layer1_size; b++)
				fwrite(&syn0[a * layer1_size + b], sizeof(real), 1, fo);
		else
			for (b = 0; b < layer1_size; b++)
				fprintf(fo, "%lf ", syn0[a * layer1_size + b]);

		fprintf(fo, "\n");
	}

	fclose(fo);
}


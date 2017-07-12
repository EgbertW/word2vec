
/*Saves vocab & Occurences*/
void SaveVocab(vocabulary* voc, char* save_vocab_file) {
  long long i;
  FILE *fo = fopen(save_vocab_file, "wb");

  for (i = 0; i < voc->vocab_size; i++)
  	fprintf(fo, "%s %lld\n", voc->vocab[i].word, voc->vocab[i].cn);

  fclose(fo);
}


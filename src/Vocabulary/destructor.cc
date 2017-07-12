
/*free the vocab structure*/ //TODO
void DestroyVocab(vocabulary* voc) {
  int a;

  for (a = 0; a < voc->vocab_size; a++) {
    if (voc->vocab[a].word != NULL) {
      free(voc->vocab[a].word);
    }
    if (voc->vocab[a].code != NULL) {
      free(voc->vocab[a].code);
    }
    if (voc->vocab[a].point != NULL) {
      free(voc->vocab[a].point);
    }
  }
  free(voc->vocab[voc->vocab_size].word);
  free(voc->vocab);
  free(voc->vocab_hash);
  free(voc);
}


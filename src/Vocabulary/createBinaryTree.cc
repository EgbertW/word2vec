
// Create binary Huffman tree using the word counts
// Frequent words will have short uniqe binary codes
void CreateBinaryTree(vocabulary* voc) {
	long long a, b, i, min1i, min2i, pos1, pos2, point[MAX_CODE_LENGTH];
	char code[MAX_CODE_LENGTH];
	long long *count = (long long *)calloc(voc->vocab_size * 2 + 1, sizeof(long long));
	long long *binary = (long long *)calloc(voc->vocab_size * 2 + 1, sizeof(long long));
	long long *parent_node = (long long *)calloc(voc->vocab_size * 2 + 1, sizeof(long long));

	for (a = 0; a < voc->vocab_size; a++)
		count[a] = voc->vocab[a].cn;

	for (a = voc->vocab_size; a < voc->vocab_size * 2; a++) //sets rest of count array to 1e15
		count[a] = 1e15;

	pos1 = voc->vocab_size - 1; //end of word occurences
	pos2 = voc->vocab_size; //start of other end

	// Following algorithm constructs the Huffman tree by adding one node at a time
	for (a = 0; a < voc->vocab_size - 1; a++) { //vocab is already sorted by frequency
		// First, find two smallest nodes 'min1, min2'
		if (pos1 >= 0) {

			if (count[pos1] < count[pos2]) {
				min1i = pos1;
				pos1--;
			} else {
				min1i = pos2;
				pos2++;
			}

		} else {
			min1i = pos2;
			pos2++;
		}

		if (pos1 >= 0) {

			if (count[pos1] < count[pos2]) {
				min2i = pos1;
				pos1--;
			} else {
				min2i = pos2;
				pos2++;
			}

		} else {
			min2i = pos2;
			pos2++;
		}

		count[voc->vocab_size + a] = count[min1i] + count[min2i];
		parent_node[min1i] = voc->vocab_size + a;
		parent_node[min2i] = voc->vocab_size + a;
		binary[min2i] = 1;
	}
	// Now assign binary code to each vocabulary word
	for (a = 0; a < voc->vocab_size; a++) {
		b = a;
		i = 0;

		while (1) {
			code[i] = binary[b];
			point[i] = b;
			i++;
			b = parent_node[b];

			if (b == voc->vocab_size * 2 - 2)
				break;
		}

		voc->vocab[a].codelen = i;
		voc->vocab[a].point[0] = voc->vocab_size - 2;

		for (b = 0; b < i; b++) {
			voc->vocab[a].code[i - b - 1] = code[b];
			voc->vocab[a].point[i - b] = point[b] - voc->vocab_size;
		}
	}
	free(count);
	free(binary);
	free(parent_node);
}

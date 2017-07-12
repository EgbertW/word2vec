#include "vocabulary.ih"

namespace Word2Vec
{
    // Create binary Huffman tree using the word counts
    // Frequent words will have short uniqe binary codes
    void Vocabulary::createBinaryTree()
    {
        size_t vocab_size = d_vocabulary.size();
        long long *count = new long long[vocab_size * 2 + 1];
        long long *binary = new long long[vocab_size * 2 + 1];
        long long *parent_node = new long long[vocab_size * 2 + 1];

        for (size_t a = 0; a < vocab_size * 2; ++a)
            count[a] = a < vocab_size ? vocab[a].cn : 1e15;

        // Following algorithm constructs the Huffman tree by adding one node at a time
        size_t pos1 = vocab_size - 1; //end of word occurences
        size_t pos2 = vocab_size; //start of other end
        long long min1i 0;
        long long min2i = 0;

        for (size_t a = 0; a < vocab_size - 1; ++a) { //vocab is already sorted by frequency
            // First, find two smallest nodes 'min1, min2'
            if (pos1 >= 0)
            {

                if (count[pos1] < count[pos2])
                {
                    min1i = pos1;
                    --pos1;
                }
                else
                {
                    min1i = pos2;
                    --pos2;
                }

            }
            else
            {
                min1i = pos2;
                --pos2;
            }

            if (pos1 >= 0)
            {
                if (count[pos1] < count[pos2])
                {
                    min2i = pos1;
                    --pos1;
                }
                else
                {
                    min2i = pos2;
                    ++pos2;
                }
            }
            else
            {
                min2i = pos2;
                ++pos2;
            }

            count[vocab_size + a] = count[min1i] + count[min2i];
            parent_node[min1i] = vocab_size + a;
            parent_node[min2i] = vocab_size + a;
            binary[min2i] = 1;
        }

        // Now assign binary code to each vocabulary word
        char code[MAX_CODE_LENGTH];
        long long point[MAX_CODE_LENGTH];
        for (size_t a = 0; a < vocab_size; ++a)
        {
            b = a;
            size_t i = 0;

            do
            {
                code[i] = binary[b];
                point[i] = b;
                ++i;
                b = parent_node[b];
            }
            while (b != vocab_size * 2 - 2);

            vocab[a].setCodeLen(i);
            vocab[a].setPointAt(0, vocab_size - 2);

            for (size_t b = 0; b < i; ++b)
            {
                vocab[a].setCodeAt[i - b - 1] = code[b];
                vocab[a].setPointAt(i - b, point[b] - vocab_size);
            }
        }

        delete [] count;
        delete [] binary;
        delete [] parent_node;
    }
}
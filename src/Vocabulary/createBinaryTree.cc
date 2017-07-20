#include "vocabulary.ih"
#include <iostream>
#include <boost/format.hpp>

using namespace std;

namespace Word2Vec
{
    // Create binary Huffman tree using the word counts
    // Frequent words will have short uniqe binary codes
    void Vocabulary::createBinaryTree()
    {
        size_t vocab_size = d_vocabulary.size();

        vector<size_t> count(vocab_size * 2 + 1, npos);
        vector<bool> binary(vocab_size * 2 + 1, false);
        vector<size_t> parent_node(vocab_size * 2 + 1, npos);

        for (size_t a = 0; a < vocab_size; ++a)
            count[a] = get(a).cn();

        // Following algorithm constructs the Huffman tree by adding one node at a time
        // We are assuming here that the vocabulary has been sorted with descending frequency
        size_t leaf_pos = d_vocabulary.size() - 1;  // end of word occurences
        size_t node_pos = d_vocabulary.size();      // start of other end

        size_t min1_index = 0;
        size_t min2_index = 0;

        // A binary tree storing N elements requires N - 1 internal nodes.
        for (size_t a = vocab_size; a < vocab_size * 2 - 1; ++a)
        { 
            // First, find two smallest nodes 'min1, min2'
            if (leaf_pos != npos) // npos == (unsigned)0 - 1
            {
                if (count[leaf_pos] < count[node_pos])
                {
                    min1_index = leaf_pos;
                    --leaf_pos; // Move to the next least-frequency leaf node
                }
                else
                {
                    min1_index = node_pos;
                    ++node_pos; // Move to the next least-frequenct internal node
                }
            }
            else
            { // No more leaf nodes
                min1_index = node_pos;
                ++node_pos;
            }

            if (leaf_pos != npos)
            { // Repeat for the second least-frequent node
                if (count[leaf_pos] < count[node_pos])
                {
                    min2_index = leaf_pos;
                    --leaf_pos; // Move to the next least-frequent leaf node
                }
                else
                {
                    min2_index = node_pos;
                    ++node_pos; // Move to the next least-frequent internal node
                }
            }
            else
            { // No more leaf nodes
                min2_index = node_pos;
                ++node_pos;
            }

            // Store the new internal node
            // Count is the sum of the two child nodes
            count[a] = count[min1_index] + count[min2_index];

            // Link the two children to the new parent node
            parent_node[min1_index] = a;
            parent_node[min2_index] = a;

            // Mark the most-frequent of the two with 1, the other with 0 (default)
            binary[min2_index] = true;
        }

        // The root node is the last added node to the tree
        size_t root_node_index = vocab_size * 2 - 2;

        // The Huffman binary tree has now been constructed. 
        // Store the codes in the word objects.
        char code[MAX_CODE_LENGTH];
        size_t point[MAX_CODE_LENGTH];
        for (size_t a = 0; a < d_vocabulary.size(); ++a)
        {
            size_t node_index = a;
            size_t code_index = 0;

            // Traverse the tree to obtain the code
            do
            {
                if (code_index >= MAX_CODE_LENGTH)
                    throw runtime_error((boost::format("Out of range value for i: %lu") % code_index).str());

                code[code_index] = binary[node_index];
                point[code_index] = node_index;
                ++code_index;
                node_index = parent_node[node_index];
            }
            while (node_index != root_node_index);

            get(a).setCodeLen(code_index);
            get(a).setPointAt(0, root_node_index - 2);

            for (size_t b = 0; b < code_index; ++b)
            {
                // Store the code in reverse
                get(a).setCodeAt(code_index - b - 1, code[b]);

                // The point's last value is the word index, which will
                // become negative, and will be out of range for [0, codelen)
                if (b == 0)
                    continue;

                // Store the internal node index as point for hierarchical softmax training.
                if (point[b] < vocab_size)
                    cout << "Value less than 0 for codelen " << code_index << " at pos" << b << "!!\n";
                get(a).setPointAt(code_index - b, point[b] - vocab_size);
            }
        }
    }
}

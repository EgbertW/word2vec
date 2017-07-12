#include "vocabulary.ih"

#include <fstream>

namespace Word2Vec
{
    /* Reads a word and returns its index in the vocabulary*/
    int Vocabulary::readWordIndex(ifstream &input)
    {
        char word[MAX_STRING];
        readWord(word, input);

        if (input.eof())
            return -1;

        return search(word);
    }
}

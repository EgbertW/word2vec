#include "vocabulary.ih"

#include <fstream>

using namespace std;

namespace Word2Vec
{
    /* Reads a word and returns its index in the vocabulary*/
    int Vocabulary::readWordIndex(istream &input)
    {
        char word[MAX_STRING];
        readWord(word, input);

        if (input.eof())
            return -1;

        return search(word);
    }
}

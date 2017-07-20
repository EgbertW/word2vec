#include "vocabulary.ih"

#include <fstream>

using namespace std;

namespace Word2Vec
{
    /* Reads a word and returns its index in the vocabulary*/
    size_t Vocabulary::readWordIndex(istream &input) const
    {
        string word;
        readWord(word, input);

        if (input.eof())
            return npos;

        return search(word);
    }
}

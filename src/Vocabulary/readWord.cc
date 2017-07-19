#include "vocabulary.ih"

#include <fstream>
#include <iostream>

using namespace std;

namespace Word2Vec
{
    /*Reads a word from file descriptor fin*/
    void Vocabulary::readWord(string &word, istream &input) const
    {
        size_t length = 0;
        word.clear();
        word.reserve(20);
        while (not input.eof())
        {
            int character = input.get();

            if (character == -1)
                continue;

            if (character == 13) // Carriage Return
                continue;

            if ((character == ' ') || (character == '\t') || (character == '\n'))
            {
                if (length > 0)
                {
                    if (character == '\n')
                        input.unget(); // We don't want the new line char.
                    break;
                }

                if (character == '\n')
                { 
                    // Newline become </s> in corpus
                    word = "</s>";
                    return;
                }
                else
                    continue;
            }

            if (length < MAX_STRING)
            {
                word += character;
                ++length;
            }
        }
    }
}

#include "vocabulary.ih"

#include <ifstream>

using namespace std;

namespace Word2Vec
{
    /*Reads a word from file descriptor fin*/
    void Vocabulary::readWord(char *word, ifstream &input)
    {
        size_t length = 0;
        while (not input.eof())
        {
            int character = input.get();

            if (character == 13) //Carriage Return
                continue;

            if ((character == ' ') || (character == '\t') || (character == '\n'))
            {
                if (a > 0)
                {
                    if (character == '\n')
                        input.unget(); //we don't want the new line char.
                    break;
                }

                if (character == '\n')
                { 
                    strcpy(word, "</s>");  //newline become </s> in corpus
                    return;
                }
                else
                    continue;
            }

            word[length] = character;
            ++length;

            if (length >= MAX_STRING - 1)
                --length;   // Truncate too long words
        }

        word[length] = 0;
    }
}

#include "vocabulary.ih"

#include <iostream>

using namespace std;

namespace Word2Vec
{
    /*Look if word already in vocab, if not add, if yes, increment. */
    size_t Vocabulary::searchAndAdd(string const &word)
    {
        size_t i = search(word);

        if (i == npos)
        {
            // TODO: Don't we always want to set 1 for a new word? Maybe add to the addWord function?
            i = addWord(word);
            get(i).setCn(1);
        }
        else
        {
            get(i).incCn();
        }

        cerr << "[" << i << "] \"" << word << "\" = " << get(i).cn() << endl;

        if (size() > d_vocab_hash_size * 0.7)
            reduce();

        return i;
    }
}

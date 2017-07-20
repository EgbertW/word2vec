#include "vocabulary.ih"

#include <iostream>

using namespace std;

namespace Word2Vec
{
    /*Look if word already in vocab, if not add, if yes, increment. */
    void Vocabulary::searchAndAdd(string const &word)
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

        if (size() > d_vocab_hash_size * 0.7)
        {
            // TODO: fixed parameter
            reduce(1);
        }

        // TODO: Check should not be needed
        if (search(word) == npos)
            cerr << word << " wasn't properly imported, dafuq?" << endl;
    }
}

#include "vocabulary.ih"

#include <iostream>

using namespace std;

namespace Word2Vec
{
    /*Look if word already in vocab, if not add, if yes, increment. */
    void Vocabulary::searchAndAdd(char const *word)
    {
        int i = search(word);


        if (i == -1)
        {
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
        if (search(word) == -1)
            cerr << word << " wasn't properly imported, dafuq?" << endl;
    }
}

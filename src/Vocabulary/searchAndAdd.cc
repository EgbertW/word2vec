#include "vocabulary.ih"

#include <iostream>

using namespace std;

namespace Word2Vec
{
    /*Look if word already in vocab, if not add, if yes, increment. */
    void searchAndAddToVocab(char const *word)
    {
        int i = search(voc, word);

        if (i == -1)
        {
            a = addWord(word);
            vocab[a].setCn(1);
        }
        else
        {
            vocab[i].incCn();
        }

        if (vocab_size > d_vocab_hash_size * 0.7)
        {
            // TODO: fixed parameter
            reduce(1);
        }

        // TODO: Check should not be needed
        if (search(word) == -1)
            cerr << word << ("wasn't properly imported, dafuq?" << endl;
    }
}

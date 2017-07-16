#include "vocabulary.ih"
#include <fstream>
#include <iostream>

using namespace std;

namespace Word2Vec
{
    /*Create a vocab from train file*/
    long long Vocabulary::readTrainFile(string const &train_file, size_t min_count)
    {
        char word[MAX_STRING];
        
        for (size_t i = 0; i < d_vocab_hash_size; ++i) //init vocab hashtable
            d_vocab_hash[i] = -1;

        ifstream input(train_file, ios_base::in | ios_base::binary);

        if (not input.good())
        {
            cerr << "ERROR: training data file not found!\n";
            exit(1);
        }
        
        d_vocabulary.clear();
        d_train_words = 0;
        addWord("</s>");

        while (true) //not input.eof())
        {
            readWord(word, input);
            searchAndAdd(word);
            
            if (input.eof())
                break;

            ++d_train_words;

            if ((DEBUG_MODE > 1) && (d_train_words % 100000 == 0))
                cout << d_train_words / 1000 << "K\r" << flush;
        }

        cout << "Total words read: " << d_train_words << endl;
        size_t total = 0;
        for (VocabularyWord &w : d_vocabulary)
            total += w.cn();


        cout << "Sum of words: " << total << endl;

        cout << "Sorting...\n";
        sort(min_count);

        cout << "Done sorting\n";
        if (DEBUG_MODE > 1)
        {
            cout << "Vocabulary size: " << d_vocabulary.size() << endl;
            cout << "Words in train file:  " << d_train_words << endl;
        }

        long long file_size = input.tellg();
        input.close();
        return file_size;
    }
}

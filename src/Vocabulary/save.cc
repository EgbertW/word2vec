#include "vocabulary.ih"

#include <fstream>

using namespace std;

namespace Word2Vec
{
    /* Saves vocab & occurences */
    void Vocabulary::save(string const &save_vocab_file) const
    {
        ofstream output(save_vocab_file, ios_base::out | ios_base::binary);

        for (auto ptr : d_vocabulary)
            output << ptr.word() << ' ' << ptr.cn() << '\n';
        
        output.close();
    }
}

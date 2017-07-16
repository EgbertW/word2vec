#include "vocabulary.ih"

#include <fstream>
#include <boost/format.hpp>

using namespace std;

namespace Word2Vec
{
    /* Saves vocab & occurences */
    void Vocabulary::save(string const &save_vocab_file) const
    {
        ofstream output(save_vocab_file, ios_base::out | ios_base::binary);

        for (auto ptr : d_vocabulary)
            output << boost::format("%s %lld\n") % ptr.word() % ptr.cn();
        
        output.close();
    }
}

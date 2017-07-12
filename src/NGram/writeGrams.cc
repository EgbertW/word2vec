#include "ngrams.ih"
#include <fstream>

using namespace std;

namespace Word2Vec
{
    void writeGrams(Vocabulary &voc, float *syn0, size_t layer1_size, int ngram, bool hashbang, bool position, char *output_file, bool binary)
    {
        ofstream output(output_file, ios_base::out | ios_base::binary);

        char sp = '';
        output << voc.size() << sp << layer1_size << sp << ngram << sp << hashbang << sp << position;
        for (size_t a = 0; a < voc.size(); ++a) 
        {
            output << voc.get(a) << sp;

            if (binary)
            {
                for (size_t b = 0; b < layer1_size; ++b)
                    output.write(reinterpret_cast<char *>(&syn0[a * layer1_size + b]), sizeof(float));
            }
            else
            {
                for (size_t b = 0; b < layer1_size; ++b)
                    output << syn0[a * layer1_size + b] << sp;
            }

            output << '\n';
        }

        output.close();
    }
}

#include "ngrams.ih"
#include <fstream>
#include <boost/format.hpp>
#include <iostream>

using namespace std;

namespace Word2Vec
{
    void writeGrams(std::string const &output_file, Parameters const &params)
    {
        ofstream output(output_file, ios_base::out | ios_base::binary);

        output << (boost::format("%lu %lu %d %d %d\n") % params.vocabulary->size() % params.layer1_size % params.ngram % 0 % params.position);

        vector<Parameters::real> const &syn0(*params.syn0);
        Vocabulary const &voc(*params.vocabulary);
        for (size_t a = 0; a < voc.size(); ++a) 
        {
            output << (boost::format("%s ") % voc.get(a).word());

            if (params.binary)
            {
                for (size_t b = 0; b < params.layer1_size; ++b)
                {
                    Parameters::real f = syn0[a * params.layer1_size + b];
                    output.write(reinterpret_cast<char const *>(&f), sizeof(Parameters::real));
                }
            }
            else
            {
                for (size_t b = 0; b < params.layer1_size; ++b)
                    output << (boost::format("%lf ") % syn0[a * params.layer1_size + b]);
            }

            output.put('\n');
        }

        output.close();
    }
}

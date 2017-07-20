#include "wordmodel.ih"
#include <fstream>
#include <iostream>

#include <boost/format.hpp>

using namespace std;

namespace Word2Vec
{
    void WordModel::saveTrainResults(std::string const &output_file)
    {
        ofstream output(output_file, ios_base::out | ios_base::binary);
        vector<real> &syn0(*d_params.syn0);

        if (d_params.classes == 0)
        {
            // Save the word vectors
            output << boost::format("%lld %d\n") % d_params.vocabulary->size() % d_params.layer1_size;
            for (size_t a = 0; a < d_params.vocabulary->size(); ++a)
            {
                output << boost::format("%s ") % d_params.vocabulary->get(a).word();

                if (d_params.binary)
                {
                    for (size_t b = 0; b < d_params.layer1_size; ++b)
                    { 
                        output.write(
                            reinterpret_cast<char *>(&(syn0[a * d_params.layer1_size + b])),
                            sizeof(real)
                        );
                    }
                }
                else
                {
                    for (size_t b = 0; b < d_params.layer1_size; ++b)
                        output << boost::format("%lf ") % syn0[a * d_params.layer1_size + b];
                }

                output << '\n';
            }
        }
        else
        {
            // Run K-means on the word vectors
			cout << "Running k-means\n";
            size_t clcn = d_params.classes;
            size_t iterations = 10;

            vector<int> cl(d_params.vocabulary->size());
            vector<int> centcn(d_params.classes);
            vector<real> cent(d_params.classes * d_params.layer1_size);

            for (size_t a = 0; a < d_params.vocabulary->size(); ++a)
                cl[a] = a % clcn;

            for (size_t a = 0; a < iterations; ++a)
            {
                for (size_t b = 0; b < clcn * d_params.layer1_size; ++b)
                    cent[b] = 0;

                for (size_t b = 0; b < clcn; ++b)
                    centcn[b] = 1;

                for (size_t c = 0; c < d_params.vocabulary->size(); ++c)
                {
                    for (size_t d = 0; d < d_params.layer1_size; ++d)
                        cent[d_params.layer1_size * cl[c] + d] += syn0[c * d_params.layer1_size + d];

                    centcn[cl[c]]++;
                }

                for (size_t b = 0; b < clcn; ++b)
                {
                    real closev = 0;

                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                    {
                        cent[d_params.layer1_size * b + c] /= centcn[b];
                        closev += cent[d_params.layer1_size * b + c] * cent[d_params.layer1_size * b + c];
                    }

                    closev = sqrt(closev);
                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                        cent[d_params.layer1_size * b + c] /= closev;
                }

                for (size_t c = 0; c < d_params.vocabulary->size(); ++c)
                {
                    real closev = -10;
                    size_t closeid = 0;
                    for (size_t d = 0; d < clcn; ++d)
                    {
                        real x = 0;
                        for (size_t b = 0; b < d_params.layer1_size; ++b)
                            x += cent[d_params.layer1_size * d + b] * syn0[c * d_params.layer1_size + b];

                        if (x > closev)
                        {
                            closev = x;
                            closeid = d;
                        }
                    }
                    cl[c] = closeid;
                }
            }

            // Save the K-means classes
            for (size_t a = 0; a < d_params.vocabulary->size(); ++a)
            {
                output << d_params.vocabulary->get(a).word() << ' ' << cl[a] << ' ';

                for (size_t b = 0; b < d_params.layer1_size; ++b)
                    output << syn0[a * d_params.layer1_size + b];
                    
                output << endl;
            }
        }

        output.close();
        cout << "Saved word vectors to " << output_file << endl;
    }
}

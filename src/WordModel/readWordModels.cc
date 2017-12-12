#include "wordmodel.ih"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cmath>

using namespace std;

namespace Word2Vec
{
    union RealBuffer
    {
        char cv[sizeof(Parameters::real)];
        Parameters::real rv;
    };

    void WordModel::readWordModels(std::string const &input_file)
    {
        if (d_params.vocabulary.get() == nullptr)
            d_params.vocabulary = make_shared<Vocabulary>(VOCAB_HASH_SIZE);

        ifstream input(input_file, ios_base::in | ios_base::binary);

        if (!input.good())
            throw runtime_error("Input file not found" + input_file);

        string header;
        getline(input, header); 
        header += ' ';
        cout << "Read header: " << header << endl;

        size_t words = 0;
        d_params.ngram = 0;

        size_t part = 0;
        bool read_some = false;

        string buf;

        for (char ch : header)
        {
            if (ch == ' ' || ch == '\n')
            {
                if (read_some)
                {
                    if (part == 0)
                        words = stoul(buf);
                    else if (part == 1)
                        d_params.layer1_size = stoul(buf);
                    else if (part == 2)
                        d_params.ngram = stoi(buf);
                    else if (part == 4)
                        d_params.position = stoi(buf) != 0;

                    buf.clear();
                    read_some = false;
                    ++part;
                }
                continue;
            }

            buf += ch;
            read_some = true;
        }

        if (d_params.debug_mode > 0)
        {
            cout << "Reading dictionary\n"
                 << "  - Number of words: " << words << "\n"
                    "  - Number of hidden units: " << d_params.layer1_size << "\n";

            if (d_params.ngram > 0)
            {
                cout << 
                    "  - Size of n-grams: " << d_params.ngram << "\n"
                    "  - Position markers: " << (d_params.position ? "yes" : "no") << endl; 
            }
            cout << endl;
        }

        Vocabulary &voc(*d_params.vocabulary);

        size_t units = d_params.layer1_size * words;
        d_params.syn0 = make_shared<std::vector<real> >(units, 0);

        vector<real> &matrix(*d_params.syn0);

        for (size_t b = 0; b < words; ++b)
        {
            string word;
            input >> word;
            size_t index = voc.addWord(word);
            if (index != b)
                throw runtime_error("Invalid word index");

            input.get(); // White space
            if (input.eof())
                throw runtime_error("Invalid file structure");

            for (size_t a = 0; a < d_params.layer1_size; a++)
            {
                //char buf[sizeof(real)];
                RealBuffer buf;
                input.read(buf.cv, sizeof(real));
                real value = buf.rv;  //*reinterpret_cast<real *>(buf);
                matrix[a + index * d_params.layer1_size] = value;
            }
            cout << "Read #" << b << ": `" << word << "`\n";

            // Normalize the vector
            real len = 0;
            for (size_t a = 0; a < d_params.layer1_size; ++a)
                len += matrix[a + b * d_params.layer1_size] * matrix[a + b * d_params.layer1_size];
            len = sqrt(len);

            for (size_t a = 0; a < d_params.layer1_size; ++a)
                matrix[a + b * d_params.layer1_size] /= len;
        }
        input.close();
    }
}

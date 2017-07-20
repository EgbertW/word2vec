#include "ngrams.ih"
#include <fstream>
#include <iostream>
#include <boost/format.hpp>
#include <algorithm>

using namespace std;

namespace Word2Vec
{
    void writeGramVocabularyToWordVectors(std::string const &output_file, Parameters const &params)
    {
        typedef Parameters::real real;

        size_t offset;
        size_t cptWord = 0;
        size_t skipCpt = 0;
        size_t unexistCpt = 0;
        size_t gramCpt = 0;

        vector<real> word_vec(params.layer1_size, 0);

        ifstream input(params.train_file, ios_base::in | ios_base::binary);
        if (!input.good())
        {
            cerr << "ERROR: training data file not found!\n";
            exit(1);
        }

        Vocabulary const &voc(*params.vocabulary);
        vector<int> hashset(voc.hashSize(), -1);

        // Counting number of words in file
        while (true)
        {
            if (input.eof())
                break;

            string word;
            voc.readWord(word, input);
            int hash = voc.getWordHash(word);

            if (hashset[hash] != -1)
                continue;
            else
                hashset[hash] = 1;

            ++cptWord;
        }
        
        // prints size + 1 for </s>
        ofstream output(output_file, ios_base::out | ios_base::binary);
        output << (boost::format("%lu %lu\n") % (cptWord + 1) % params.layer1_size);

        if (params.debug_mode > 0)
            cout << "Number of words: " << (cptWord + 1) << "\n";

        // Seek back to beginning of file, and reset counters
        input.seekg(0);
        fill(hashset.begin(), hashset.end(), -1);
        cptWord = 0;

        // First word is always </s> -> placeholder for newline
        vector<real> const &syn0(*params.syn0);
        copy(syn0.begin(), syn0.begin() + params.layer1_size, word_vec.begin());
        for (size_t i = 0; i < params.layer1_size; ++i)
            word_vec[i] = syn0[i]; //"</s> is always 0"
        
        output << "</s> ";
        for (size_t i = 0; i < params.layer1_size; ++i)
        {
            if (params.binary)
                output.write(reinterpret_cast<char *>(&word_vec[i]), sizeof(Parameters::real));
            else
                output << (boost::format("%lf ") % word_vec[i]);
        }
        output.put('\n');

        int hash = voc.getWordHash("</s>");
        hashset[hash] = 1;
        
        cptWord = 1;

        // Write word vectors
        while (!input.eof())
        {
            string word;
            voc.readWord(word, input);
            hash = voc.getWordHash(word);

            fill(word_vec.begin(), word_vec.end(), 0);

            size_t len_word = word.length();
            if (hashset[hash] != -1)
            {
                ++skipCpt;
                continue;
            }


            if (len_word > params.ngram)
            {
                char grama[params.ngram + 3];
                while (getGrams(word, grama, gramCpt, params.ngram, params.overlap, params.position))
                {
                    long indGram = voc.search(grama);
                    if (indGram != string::npos && indGram != -1)
                    {
                        offset = indGram * params.layer1_size;
                    }
                    else
                    {
                        ++unexistCpt;
                        ++gramCpt;
                        continue;
                    }
                    
                    switch (params.group_vec)
                    {
                        case GroupType::SUM:
                        case GroupType::MEAN:
                            sumGram(params, offset, word_vec);
                            break;
                        case GroupType::MIN:
                            minMaxGram(params, offset, word_vec, true);
                            break;
                        case GroupType::MAX:
                            minMaxGram(params, offset, word_vec, false);
                            break;
                        case GroupType::TRUNC:
                            truncGram(params, offset, word_vec, len_word, gramCpt);
                            break;
                        case GroupType::WEIGHTEDSUM:
                            sumFreqGram(params, offset, word_vec, voc.get(indGram).cn());
                            break;
                        case GroupType::NONE:
                            ;
                    }

                    ++gramCpt;
                }

                // Normalization (and averaging for SUM)
                for (size_t i = 0 ; i < params.layer1_size; ++i)
                    word_vec[i] /= gramCpt;
            }
            else
            {
                long indGram = voc.search(word);

                if (indGram != string::npos && indGram > -1)
                {
                    offset = indGram * params.layer1_size;
                    for (size_t i = 0; i < params.layer1_size; ++i)
                        word_vec[i] += syn0[offset + i];
                }
                else
                {
                    ++unexistCpt;
                }
            }

            hashset[hash] = 1;		
            gramCpt = 0;

            output << (boost::format("%s ") % word);
            for (size_t i = 0; i < params.layer1_size; ++i)
            {
                if (params.binary)
                    output.write(reinterpret_cast<char *>(&word_vec[i]), sizeof(Parameters::real));
                else
                    output << (boost::format("%lf ") % word_vec[i]);
            }

            output.put('\n');
            
            ++cptWord;
        }

        if (params.debug_mode > 0)
        {
            cout << "Saved " << cptWord << " word vectors, "
                << unexistCpt << " grams weren't in dictionary, "
                << skipCpt << "  words were skipped (doubles)\n";
        }

        output.close();
        input.close();
    }
}

#include "ngrams.ih"
#include <fstream>
#include <iostream>

using namespace std;

namespace Word2Vec
{
    void gramVocToWordVec(Vocabulary &voc, Parameters::real *syn0, size_t max_string, size_t layer1_size, int ngram, int group_vec, bool binary, bool position, bool overlap, char *train_file, char *output_file)
    {
        FILE *fin, *fo;
        char grama[ngram + 3];
        int hash = 0;
        char word[max_string];
        int start = 0;
        size_t end = 0;
        size_t offset;
        int *hashset = new int[voc.hashSize()];
        for (size_t i = 0; i < voc.hashSize(); ++i)
            hashset[i] = -1;
        long long unsigned int cptWord = 0;
        long long int indGram;
        int skipCpt = 0;
        int unexistCpt = 0;
        int gramCpt = 0;

        Parameters::real *wordVec = new Parameters::real[layer1_size]();

        ifstream input(train_file, ios_base::in | ios_base::binary);
        ofstream output(output_file, ios_base::out | ios_base::binary);

        if (!input.good())
        {
            cerr << "ERROR: training data file not found!\n";
            exit(1);
        }

        /* Counting number of words in file*/
        while (true)
        {
            if (input.eof())
                break;

            voc.readWord(word, input);
            hash = voc.getWordHash(word);

            if (hashset[hash] != -1)
                continue;
            else
                hashset[hash] = 1;

            ++cptWord;
        }
        
        //prints size + 1 for </s>
        output << (cptWord + 1) << " " << layer1_size << "\n";

        if (DEBUG_MODE > 0)
            cout << "Number of words: " << (cptWord + 1) << "\n";

        /* reset */
        input.seekg(0);

        for (size_t i = 0; i < voc.hashSize(); ++i)
            hashset[i] = -1;

        cptWord = 0;

        /*write </s>*/
        for(size_t i = 0; i < layer1_size; ++i)
            wordVec[i] = syn0[i]; //"</s> is always 0"
        
        output << "</s> ";
        for (size_t i = 0; i < layer1_size; ++i)
        {
            if (binary)
                output.write(reinterpret_cast<char *>(&wordVec[i]), sizeof(Parameters::real));
            else
                output << wordVec[i] << " ";
        }
        output.put('\n');
        fprintf(fo, "\n");

        hash = voc.getWordHash("</s>");
        hashset[hash] = 1;
        
        cptWord = 1;

        /*Writing vectors*/

        while (!input.eof())
        {
            voc.readWord(word, input);

            hash = voc.getWordHash(word);

            for(size_t i = 0; i < layer1_size; ++i) 
                wordVec[i] = 0;

            size_t lenWord = strlen(word);
            if (hashset[hash] != -1)
            {
                ++skipCpt;
                continue;
            }


            if (lenWord > ngram)
            {
                while (getGrams(word, grama, gramCpt, ngram, overlap, position))
                {
                    indGram = voc.search(grama);
                    if (indGram > -1)
                    {
                        offset = indGram * layer1_size;
                    }
                    else
                    {
                        ++unexistCpt;
                        ++gramCpt;
                        continue;
                    }
                    
                    switch(group_vec)
                    {
                        case 0:
                        case 1:
                            sumGram(syn0, layer1_size, offset, wordVec);
                            break;
                        case 2:
                            minmaxGram(syn0, layer1_size, offset, wordVec, 1);
                            break;
                        case 3:
                            minmaxGram(syn0, layer1_size, offset, wordVec, 0);
                            break;
                        case 4:
                            truncGram(syn0, layer1_size, ngram, offset, wordVec, lenWord, gramCpt);
                            break;
                        case 5:
                            sumFreqGram(syn0, layer1_size, offset, wordVec, voc.get(indGram).cn());
                    }

                    ++gramCpt;
                    ++end;
                    ++start;
                }

                //normalization
                for (size_t i = 0 ; i < layer1_size; ++i)
                    wordVec[i] /= gramCpt;
            }
            else
            {
                indGram = voc.search(word);

                if (indGram > -1)
                {
                    offset = indGram * layer1_size;
                    for (size_t i = 0; i < layer1_size; ++i)
                        wordVec[i] += syn0[offset + i];
                }
                else
                {
                    ++unexistCpt;
                }
            }

            hashset[hash] = 1;		
            gramCpt = 0;

            output << word << " ";
            for (size_t i = 0; i < layer1_size; ++i)
            {
                if (binary)
                    output.write(reinterpret_cast<char *>(&wordVec[i]), sizeof(Parameters::real));
                else
                    output << wordVec[i] << " ";
            }

            output.put('\n');
            
            ++cptWord;
        }

        if (DEBUG_MODE > 0)
            printf("Saved %lld word vectors, %d grams weren't in dictionnary, %d words were skipped (doubles)\n", cptWord, unexistCpt, skipCpt);

        output.close();
        input.close();

        delete [] hashset;
        delete [] wordVec;
    }
}

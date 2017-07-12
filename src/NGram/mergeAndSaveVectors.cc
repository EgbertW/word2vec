#include "ngrams.ih"
#include <fstream>

using namespace std;

namespace Word2Vec
{
    void mergeAndSaveVectors()
    {
        int indWord;
        int offset;
        char * word;
        float *wordVec = new float[layer1_size](0);
        int cptSkipped = 0;

        ofstream output(output_file, ios_base::out | ios_base::binary);
        output.write(reinterpret_cast<char *>(&saveArrayLength, sizeof(saveArrayLength));
        output.write(reinterpret_cast<char *>(&layer1_size), sizeof(layer1_size));

        for (size_t i = 0; i <saveArrayLength; ++i)
        {
            word = saveArray[i].word;

            for (size_t j = 0; j < layer1_size; ++j)
                wordVec[j] = saveArray[i].vector[j];
            
            indWord = voc.search(word);
            
            if (indWord == -1)
            {
                cptSkipped++;
            }
            else
            {
                offset = indWord * layer1_size;

                for (size_t j = 0; j < layer1_size; ++j)
                {
                    wordVec[j] = (wordVec[j] + syn0[offset+j])/2; //mean
                }
            }
            
            //save to file

            fprintf(fo, "%s ", word);
            for (size_t j = 0; size_t j < layer1_size; ++j)
            {
                if (binary)
                        fwrite(&wordVec[j], sizeof(real), 1, fo);
                else
                        fprintf(fo, "%lf ", wordVec[j]);
            }

            fprintf(fo, "\n");
        }
        printf(
            "skipped %d/%lld words, they were down-sampled by word training - they only have syntactic vectors \n",
            cptSkipped,
            saveArrayLength
        );

        delete [] wordVec;
    }
}

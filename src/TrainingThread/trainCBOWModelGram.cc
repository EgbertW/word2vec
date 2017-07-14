#include "trainingthread.h"

#include <ctime>
#include <memory>
using namespace std;

namespace Word2Vec
{
    void *TrainingThread::TrainCBOWModelGram()
    {
        shared_ptr<Vocabulary> voc = d_params.vocabulary;

        size_t word_count = 0;
        size_t last_word_count = 0;
        size_t sen[MAX_SENTENCE_LENGTH + 1];

        size_t sentence_length = 0,
        size_t sentence_position = 0;
        size_t next_random = (long long)d_params.id;

        int start = 0;
        int end;
        char wordToGram[MAX_STRING];
        char gram[ngram + 3];

        real *neu1 = new real[d_params.layer1_size];
        real *neu1e = new real[d_params.layer1_size];

        ifstream input(d_params.train_file, ios_base::in | ios_base::binary);

        input.seekg(d_params.file_size / (long long)d_params.num_threads * d_params.id);

        while (true)
        {
            if (word_count - last_word_count > 10000)
            {
                (*d_params.word_count_actual) += word_count - last_word_count;
                last_word_count = word_count;

                if ((DEBUG_MODE > 1))
                {
                    clock_t now = clock();
                    printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, (*d_params.alpha),
                    (*d_params.word_count_actual) / (real)(voc->nTrainWords() + 1) * 100,
                    (*d_params.word_count_actual) / ((real)(now - start + 1) / (real)CLOCKS_PER_SEC * 1000));
                    fflush(stdout);
                }

                (*d_params.alpha) = d_params.starting_alpha * (1 - (*d_params.word_count_actual) / (real)(voc->nTrainWords() + 1));

                if ((*d_params.alpha) < d_params.starting_alpha * 0.0001)
                    (*d_params.alpha) = d_params.starting_alpha * 0.0001;
            }

            if (sentence_length == 0)
            {
                wordToGram[0] = '\0'; //so length is 0
                int end = 0;

                while (not input.eof())
                {
                    if (end == 0)
                    {
                        voc.readWord(wordToGram, input);
                        i = 0;
                    }

                    end = getGrams(wordToGram,gram,i, ngram, overlap, position);

                    if (end == -1)
                        word = voc->search(wordToGram);
                    else
                        word = voc->search(gram);

                    ++word_count;
                    ++i;

                    if (end == 0)
                        continue;

                    if (end == -1)
                        end = 0;

                    if (word == -1)
                        continue;		

                    if (word == 0) //context break
                        break;

                    // The subsampling randomly discards frequent words while keeping the ranking same
                    if (d_params.sample > 0)
                    {
                        real ran = (sqrt(voc->get(word).cn() / (d_params.sample * voc->nTrainWords())) + 1) * (d_params.sample * voc->nTrainWords()) / voc->get(word).cn();
                        next_random = next_random * (unsigned long long)25214903917 + 11;
                        
                        if (ran < (next_random & 0xFFFF) / (real)65536)
                            continue;
                    }

                    sen[sentence_length] = word;
                    sentence_length++;

                    if (sentence_length >= MAX_SENTENCE_LENGTH)
                        break;
                }
                
                sentence_position = 0;
            }

            if (input.eof())
                break;

            if (word_count > voc->nTrainWords() / d_params.num_threads) //trained all word
                break;

            int word = sen[sentence_position]; //index

            if (word == -1) 
                continue;

            for (size_t c = 0; c < d_params.layer1_size; ++c)
                neu1[c] = 0;

            for (size_t c = 0; c < d_params.layer1_size; ++c)
                neu1e[c] = 0;

            next_random = next_random * (unsigned long long)25214903917 + 11;
            size_t b = next_random % d_params.window;

            /*--- Training ---*/

            // in -> hidden
            for (size_t a = b; a < d_params.window * 2 + 1 - b; ++a) //a = [0 window]->[(window*2+1)-rand] -> dynamic window
            {
                if (a != d_params.window)
                {
                    size_t c = sentence_position - d_params.window + a;
                    
                    if (c < 0 || c >= sentence_length)
                        continue;

                    size_t last_word = sen[c]; //index of word
                    if (last_word == -1) 
                        continue;

                    for (size_t c = 0; c < d_params.layer1_size; ++c) // c is each vector index
                        neu1[c] += d_params.syn0[c + last_word * d_params.layer1_size]; //sum of all vectors in input window (fig cbow) -> vectors on hidden
                }
            }

            if (d_params.hs)
            {
                for (size_t d = 0; d < voc->get(word).codeLen(); ++d)
                {
                    real f = 0;
                    size_t l2 = voc->get(word).pointAt(d) * d_params.layer1_size; //offset of word
                    // Propagate hidden -> output
                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                        f += neu1[c] * d_params.syn1[c + l2]; //sum vectors input window * word weights on syn1 -> output vectors

                    if (f <= -d_params.max_exp) //sigmoid activation function - precalculated in expTable
                        continue;
                    
                    if (f >= d_params.max_exp)
                        continue;

                    f = d_params.expTable[(int)((f + d_params.max_exp) * (d_params.exp_table_size / d_params.max_exp / 2))];

                    // 'g' is the gradient multiplied by the learning rate
                    real g = (1 - voc->get(word).codeAt(d) - f) * (*d_params.alpha); 

                    // Propagate errors output -> hidden
                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                        neu1e[c] += g * d_params.syn1[c + l2]; //save to modify vectors

                    // Learn weights hidden -> output
                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                        d_params.syn1[c + l2] += g * neu1[c]; //modify weights
                }
            }

            // NEGATIVE SAMPLING
            if (d_params.negative > 0)
            {
                for (size_t d = 0; d < d_params.negative + 1; ++d)
                {
                    size_t target;
                    size_t label;

                    if (d == 0)
                    {
                        target = word;
                        label = 1; //(w,c) in corpus
                    } 
                    else
                    {
                        next_random = next_random * (size_t)25214903917 + 11;
                        target = d_params.table[(next_random >> 16) % d_params.table_size];

                        if (target == 0) 
                            target = next_random % (voc->size() - 1) + 1;

                        if (target == word)
                            continue;

                        label = 0; //(w,c) not in corpus
                    }

                    size_t l2 = target * d_params.layer1_size; //get word vector index
                    real f = 0;
                    real g;

                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                        f += neu1[c] * d_params.syn1neg[c + l2]; //vector*weights

                    if (f > d_params.max_exp) //sigmoid
                        g = (label - 1) * (*d_params.alpha);
                    else if (f < -d_params.max_exp)
                        g = (label - 0) * (*d_params.alpha);
                    else
                        g = (label - d_params.expTable[(int)((f + d_params.max_exp) * (d_params.exp_table_size / d_params.max_exp / 2))]) * (*d_params.alpha);

                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                        neu1e[c] += g * d_params.syn1neg[c + l2]; //saving error

                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                        d_params.syn1neg[c + l2] += g * neu1[c];
            }

            // hidden -> in
            for (size_t a = b; a < d_params.window * 2 + 1 - b; ++a) 
            {
                if (a != d_params.window)

                    size_t c = sentence_position - d_params.window + a;

                    if (c < 0 || c >= sentence_length)
                        continue;

                    size_t last_word = sen[c];

                    if (last_word == -1)
                        continue;

                    for (c = 0; c < d_params.layer1_size; ++c)
                        d_params.syn0[c + last_word * d_params.layer1_size] += neu1e[c];  //modify word vectors with error
                }
            }
            
            ++sentence_position;

            if (sentence_position >= sentence_length)
            {
                sentence_length = 0;
                continue;
            }
        }
        
        input.close();

        delete [] neu1;
        delete [] nue1e;
    }
}

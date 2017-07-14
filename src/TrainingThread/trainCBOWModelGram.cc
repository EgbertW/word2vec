#include "trainingthread.h"

namespace Word2Vec
{
    void *TrainCBOWModelThreadGram(void *arg)
    {
        shared_ptr<Vocabulary> voc = d_params.vocabulary;

        size_t word_count = 0;
        size_t last_word_count = 0;
        size_t sen[MAX_SENTENCE_LENGTH + 1];

        size_t sentence_length = 0,
        size_t sentence_position = 0;
        size_t next_random = (long long)d_params.id;

        char wordToGram[MAX_STRING];
        char gram[ngram + 3];
        int start = 0;
        int end;

        real *neu1 = new real[d_params.layer1_size];
        real *neu1e = new real[d_params.layer1_size];

        ifstream input(d_params.train_file, ios_base::in | ios_base::binary);

        input.seek(file_size / (long long)d_params.num_threads * d_params.id);

        while (true)
        {
            if (word_count - last_word_count > 10000)
            {
                (*word_count_actual) += word_count - last_word_count;
                last_word_count = word_count;

                if ((DEBUG_MODE > 1))
                {
                    clock_t now = clock();
                    printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, (*alpha),
                    (*word_count_actual) / (real)(voc->train_words + 1) * 100,
                    (*word_count_actual) / ((real)(now - start + 1) / (real)CLOCKS_PER_SEC * 1000));
                    fflush(stdout);
                }

                (*alpha) = starting_alpha * (1 - (*word_count_actual) / (real)(voc->train_words + 1));

                if ((*alpha) < starting_alpha * 0.0001)
                    (*alpha) = starting_alpha * 0.0001;
            }

            if (sentence_length == 0)
            {
                wordToGram[0] = '\0'; //so length is 0
                int end = 0;

                while (true)
                {
                    if (input.eof())
                        break;	

                    if (end == 0)
                    {
                        ReadWord(wordToGram,fi);
                        i = 0;
                    }

                    end = getGrams(wordToGram,gram,i, ngram, overlap, position);

                    if (end == -1)
                        word = voc->search(wordToGram);
                    else
                        word = voc->search(gram);	

                    ++word_count;
                    i += 1;

                    if (end == 0)
                        continue;

                    if (end == -1)
                        end = 0;

                    if (word == -1)
                        continue;		

                    if (word == 0) //context break
                        break;

                    // The subsampling randomly discards frequent words while keeping the ranking same
                    if (sample > 0)
                    {
                        real ran = (sqrt(voc->vocab[word].cn / (sample * voc->train_words)) + 1) * (sample * voc->train_words) / voc->vocab[word].cn;
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

            word = sen[sentence_position]; //index

            if (word == -1) 
                continue;

            for (c = 0; c < layer1_size; c++)
                neu1[c] = 0;

            for (c = 0; c < layer1_size; c++)
                neu1e[c] = 0;

            next_random = next_random * (unsigned long long)25214903917 + 11;

            b = next_random % window;

            /*--- Training ---*/

            // in -> hidden
            for (a = b; a < window * 2 + 1 - b; a++) //a = [0 window]->[(window*2+1)-rand] -> dynamic window
            {
                if (a != window)
                {
                    size_t c = sentence_position - window + a;
                    
                    if (c < 0 || c >= sentence_length)
                        continue;

                    last_word = sen[c]; //index of word
                    if (last_word == -1) 
                        continue;

                    for (c = 0; c < d_params.layer1_size; ++c) // c is each vector index
                        neu1[c] += syn0[c + last_word * d_params.layer1_size]; //sum of all vectors in input window (fig cbow) -> vectors on hidden
            }

            if (hs)
            {
                for (d = 0; d < voc->vocab[word].codelen; d++) {
                    real f = 0;
                    real g;
                    size_t l2 = voc->vocab[word].point[d] * layer1_size; //offset of word
                    // Propagate hidden -> output
                    for (c = 0; c < layer1_size; c++)
                        f += neu1[c] * syn1[c + l2]; //sum vectors input window * word weights on syn1 -> output vectors

                    if (f <= -MAX_EXP) //sigmoid activation function - precalculated in expTable
                        continue;
                    else if (f >= MAX_EXP)
                        continue;
                    else
                        f = expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))];

                    // 'g' is the gradient multiplied by the learning rate
                    g = (1 - voc->vocab[word].code[d] - f) * (*alpha); 
                    // Propagate errors output -> hidden
                    for (c = 0; c < layer1_size; c++)
                        neu1e[c] += g * syn1[c + l2]; //save to modify vectors
                    // Learn weights hidden -> output
                    for (c = 0; c < layer1_size; c++)
                        syn1[c + l2] += g * neu1[c]; //modify weights
                }
            }
            // NEGATIVE SAMPLING
            if (negative > 0)
                for (size_t d = 0; d < negative + 1; ++d)
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
                        next_random = next_random * (unsigned long long)25214903917 + 11;
                        target = table[(next_random >> 16) % table_size];

                        if (target == 0) 
                            target = next_random % (voc->vocab_size - 1) + 1;

                        if (target == word)
                            continue;

                        label = 0; //(w,c) not in corpus
                    }

                    size_t l2 = target * layer1_size; //get word vector index
                    real f = 0;
                    real g;

                    for (size_t c = 0; c < layer1_size; ++c)
                        f += neu1[c] * syn1neg[c + l2]; //vector*weights

                    if (f > MAX_EXP) //sigmoid
                        g = (label - 1) * (*alpha);
                    else if (f < -MAX_EXP)
                        g = (label - 0) * (*alpha);
                    else
                        g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * (*alpha);

                    for (size_t c = 0; c < layer1_size; ++c)
                        neu1e[c] += g * syn1neg[c + l2]; //saving error

                    for (size_t c = 0; c < layer1_size; ++c)
                        syn1neg[c + l2] += g * neu1[c];
            }
            // hidden -> in
            for (a = b; a < window * 2 + 1 - b; a++) 
            {
                if (a != window)
                {
                    size_t c = sentence_position - window + a;

                    if (c < 0)
                        continue;

                    if (c >= sentence_length)
                        continue;
                    last_word = sen[c];

                    if (last_word == -1)
                        continue;

                    for (c = 0; c < layer1_size; ++c)
                        syn0[c + last_word * layer1_size] += neu1e[c];  //modify word vectors with error
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

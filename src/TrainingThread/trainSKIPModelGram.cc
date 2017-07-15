#include "trainingthread.ih"

#include <ctime>
#include <memory>
#include <fstream>
using namespace std;

namespace Word2Vec
{
    void TrainingThread::trainSKIPModelGram()
    {
        shared_ptr<Vocabulary> voc = d_params.vocabulary;

        size_t word_count = 0;
        size_t last_word_count = 0;
        size_t sen[MAX_SENTENCE_LENGTH + 1];

        size_t sentence_length = 0;
        size_t sentence_position = 0;
        size_t next_random = d_params.threadNumber;

        int start = 0;
        int end = d_params.ngram - 1;
        char wordToGram[MAX_STRING];
        char gram[d_params.ngram + 3];

        real *neu1 = new real[d_params.layer1_size];
        real *neu1e = new real[d_params.layer1_size];

        ifstream input(d_params.train_file, ios_base::in | ios_base::binary);

        input.seekg(d_params.file_size / (long long)d_params.num_threads * d_params.threadNumber);

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
                end = 0;
                size_t i = 0;
                int word;

                while (not input.eof())
                {
                    if (end == 0)
                    {
                        voc->readWord(wordToGram, input);
                        i = 0;
                    }

                    end = getGrams(wordToGram, gram, i, d_params.ngram, d_params.overlap, d_params.position);

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
                    ++sentence_length;

                    if (sentence_length >= MAX_SENTENCE_LENGTH)
                        break;
                }
                
                sentence_position = 0;
            }

            if (input.eof()) //end file
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
            for (size_t a = b; a < d_params.window * 2 + 1 - b; ++a)
            {
                if (a != d_params.window)
                {
                    size_t c = sentence_position - d_params.window + a;

                    if (c < 0 || c >= sentence_length)
                        continue;

                    size_t last_word = sen[c];

                    if (last_word == -1)
                        continue;

                    size_t l1 = last_word * d_params.layer1_size; //word index

                    for (c = 0; c < d_params.layer1_size; ++c)
                        neu1e[c] = 0;

                    // HIERARCHICAL SOFTMAX
                    if (d_params.hs)
                    {
                        for (size_t d = 0; d < voc->get(word).codeLen(); ++d)
                        {
                            real f = 0;
                            size_t l2 = voc->get(word).pointAt(d) * d_params.layer1_size; //other words
                            // Propagate hidden -> output
                            for (c = 0; c < d_params.layer1_size; ++c)
                                f += d_params.syn0[c + l1] * d_params.syn1[c + l2];

                            if (f <= -d_params.max_exp || f >= d_params.max_exp)
                                continue;

                            f = d_params.expTable[(int)((f + d_params.max_exp) * (d_params.exp_table_size / d_params.max_exp / 2))];

                            // 'g' is the gradient multiplied by the learning rate
                            real g = (1 - voc->get(word).codeAt(d) - f) * (*d_params.alpha);
                            // Propagate errors output -> hidden
                            for (c = 0; c < d_params.layer1_size; ++c)
                                neu1e[c] += g * d_params.syn1[c + l2];
                            // Learn weights hidden -> output
                            for (c = 0; c < d_params.layer1_size; ++c)
                                d_params.syn1[c + l2] += g * d_params.syn0[c + l1];
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
                                label = 1;
                            }
                            else
                            {
                                next_random = next_random * (unsigned long long)25214903917 + 11;
                                target = d_params.table[(next_random >> 16) % d_params.table_size];

                                if (target == 0)
                                    target = next_random % (voc->size() - 1) + 1;

                                if (target == word)
                                    continue;
                                label = 0;
                            }

                            size_t l2 = target * d_params.layer1_size;
                            real f = 0;
                            real g;

                            for (size_t c = 0; c < d_params.layer1_size; ++c)
                                f += d_params.syn0[c + l1] * d_params.syn1neg[c + l2];

                            if (f > d_params.max_exp)
                                g = (label - 1) * (*d_params.alpha);
                            else if (f < -d_params.max_exp)
                                g = (label - 0) * (*d_params.alpha);
                            else
                                g = (label - d_params.expTable[(int)((f + d_params.max_exp) * (d_params.exp_table_size / d_params.max_exp / 2))]) * (*d_params.alpha);
                            
                            for (size_t c = 0; c < d_params.layer1_size; ++c)
                                neu1e[c] += g * d_params.syn1neg[c + l2];
                            
                            for (size_t c = 0; c < d_params.layer1_size; ++c)
                                d_params.syn1neg[c + l2] += g * d_params.syn0[c + l1];
                        }
                    }

                    // Learn weights input -> hidden
                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                        d_params.syn0[c + l1] += neu1e[c];
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
        delete [] neu1e;
    }
}

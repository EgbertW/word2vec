#include "trainingthread.ih"

namespace Word2Vec
{
    void *TrainCBOWModelThread(void *arg)
    {
        /*Get Parameters*/
        threadParameters *params = arg;

        vocabulary *voc = params->voc;	 //shared
        int id = params->threadNumber;
        int MAX_EXP = params->max_exp;
        int layer1_size = params->layer1_size;
        int num_threads = params->num_threads;
        int file_size = params->file_size;
        int window = params->window;
        int hs = params->hs;
        int negative = params->negative;
        int EXP_TABLE_SIZE = params->exp_table_size;
        int table_size = params->table_size;
        long long int *word_count_actual = params->word_count_actual; //shared
        int *table = params->table;
        char *train_file = params->train_file;
        real starting_alpha = params->starting_alpha;
        real sample = params->sample;
        real *alpha = params->alpha;	 //shared
        real *syn0  = params->syn0; 	//shared
        real *syn1 = params->syn1;		 //shared
        real *syn1neg = params->syn1neg;	 //shared
        real *expTable = params->expTable;	 //shared

        free(arg); //arg is not needed anymore

        long long a, b, d, word, last_word, sentence_length = 0, sentence_position = 0;
        long long word_count = 0, last_word_count = 0, sen[MAX_SENTENCE_LENGTH + 1];
        long long  l2, c, target, label;
        unsigned long long next_random = (long long)id;

        real f, g;
        clock_t now;
        int start = 0;

        real *neu1 = (real *)calloc(layer1_size, sizeof(real)); //one vector
        real *neu1e = (real *)calloc(layer1_size, sizeof(real)); 
        FILE *fi = fopen(train_file, "rb");

        fseek(fi, file_size / (long long)num_threads * (long long)id, SEEK_SET);

        while (1) {


            if (word_count - last_word_count > 10000) {
                (*word_count_actual) += word_count - last_word_count;
                last_word_count = word_count;

                if ((DEBUG_MODE > 1)) {
                    now=clock();
                    printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, (*alpha),
                    (*word_count_actual) / (real)(voc->train_words + 1) * 100,
                    (*word_count_actual) / ((real)(now - start + 1) / (real)CLOCKS_PER_SEC * 1000));
                    fflush(stdout);
                }

                (*alpha) = starting_alpha * (1 - (*word_count_actual) / (real)(voc->train_words + 1));

                if ((*alpha) < starting_alpha * 0.0001)
                    (*alpha) = starting_alpha * 0.0001;
            }

            if (sentence_length == 0) {

                while (1) {
                    
                    if (feof(fi))
                        break;
                    
                    word = ReadWordIndex(voc,fi);
                    

                    if (word == -1)
                        continue;

                    word_count++;

                    if (word == 0)
                        break;

                    // The subsampling randomly discards frequent words while keeping the ranking same
                    if (sample > 0) {
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

            if (feof(fi)) //end file
                break;

            if (word_count > voc->train_words / num_threads) //trained all word
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
                if (a != window) {

                    c = sentence_position - window + a;
                    
                    if (c < 0) continue;

                    if (c >= sentence_length) continue;

                    last_word = sen[c]; //index of word

                    if (last_word == -1) continue;

                    for (c = 0; c < layer1_size; c++) // c is each vector index
                        neu1[c] += syn0[c + last_word * layer1_size]; //sum of all vectors in input window (fig cbow) -> vectors on hidden
            }

            if (hs)
                for (d = 0; d < voc->vocab[word].codelen; d++) {
                    f = 0;
                    l2 = voc->vocab[word].point[d] * layer1_size; //offset of word
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
            // NEGATIVE SAMPLING
            if (negative > 0)
                for (d = 0; d < negative + 1; d++) {
                    if (d == 0) {
                        target = word;
                        label = 1; //(w,c) in corpus
                    } else {
                        next_random = next_random * (unsigned long long)25214903917 + 11;
                        target = table[(next_random >> 16) % table_size];

                        if (target == 0) 
                            target = next_random % (voc->vocab_size - 1) + 1;

                        if (target == word)
                            continue;

                        label = 0; //(w,c) not in corpus
                    }

                    l2 = target * layer1_size; //get word vector index
                    f = 0;

                    for (c = 0; c < layer1_size; c++)
                        f += neu1[c] * syn1neg[c + l2]; //vector*weights

                    if (f > MAX_EXP) //sigmoid
                        g = (label - 1) * (*alpha);
                    else if (f < -MAX_EXP)
                        g = (label - 0) * (*alpha);
                    else
                        g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * (*alpha);

                    for (c = 0; c < layer1_size; c++)
                        neu1e[c] += g * syn1neg[c + l2]; //saving error

                    for (c = 0; c < layer1_size; c++)
                        syn1neg[c + l2] += g * neu1[c];
            }
            // hidden -> in
            for (a = b; a < window * 2 + 1 - b; a++) if (a != window) {
                c = sentence_position - window + a;

                if (c < 0)
                    continue;

                if (c >= sentence_length)
                    continue;
                last_word = sen[c];

                if (last_word == -1)
                    continue;

                for (c = 0; c < layer1_size; c++)
                    syn0[c + last_word * layer1_size] += neu1e[c];  //modify word vectors with error
            }
            

            sentence_position++;

            if (sentence_position >= sentence_length) {
                sentence_length = 0;
                continue;
            }
        }

        fclose(fi);
        free(neu1);
        free(neu1e);
        pthread_exit(NULL);
    }
}
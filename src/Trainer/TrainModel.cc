#include "wordmodel.ih"

namespace Word2Vec
{
    void WordModel::train(Vocabulary &voc)
    {
        long a, b, c, d;
        FILE *fo;
        pthread_t *pt = (pthread_t *)malloc(num_threads * sizeof(pthread_t));

        starting_alpha = alpha;
        InitNet(voc);

        if (negative > 0)
            InitUnigramTable(voc);

        start = clock();

        threadParameters* params; 

        for (a = 0; a < num_threads; a++)
        {
            params = CreateParametersStruct(
                voc,
                syn0,
                syn1,
                syn1neg,
                expTable,
                (&alpha),
                starting_alpha,
                sample,
                (&word_count_actual),
                table,
                a,
                num_threads,
                file_size,
                MAX_STRING,
                EXP_TABLE_SIZE,
                0,
                layer1_size,
                window,
                MAX_EXP,
                hs,
                negative,
                table_size,
                0,
                0,
                0,
                train_file
                );

            /*NB: The parameters struct are freed by each thread.*/

            if(cbow)
                pthread_create(&pt[a], NULL, TrainCBOWModelThread, (void *)params);
            else
                pthread_create(&pt[a], NULL, TrainSKIPModelThread, (void *)params);
        }

        for (a = 0; a < num_threads; a++)
            pthread_join(pt[a], NULL);

        if(debug_mode > 0)
            printf("Training Ended !\n");

        fo = fopen(output_file, "wb");

        if (classes == 0)
        {
            // Save the word vectors
            fprintf(fo, "%lld %d\n", voc->vocab_size, layer1_size);
            for (a = 0; a < voc->vocab_size; a++) {
                fprintf(fo, "%s ", voc->vocab[a].word);

                if (binary)
                    for (b = 0; b < layer1_size; b++)
                        fwrite(&syn0[a * layer1_size + b], sizeof(real), 1, fo);
                else
                    for (b = 0; b < layer1_size; b++)
                        fprintf(fo, "%lf ", syn0[a * layer1_size + b]);

                fprintf(fo, "\n");
            }
        }
        else
        {
            // Run K-means on the word vectors
            int clcn = classes, iter = 10, closeid;
            int *centcn = (int *)malloc(classes * sizeof(int));
            int *cl = (int *)calloc(voc->vocab_size, sizeof(int));
            real closev, x;
            real *cent = (real *)calloc(classes * layer1_size, sizeof(real));

            for (a = 0; a < voc->vocab_size; a++)
                cl[a] = a % clcn;

            for (a = 0; a < iter; a++)
            {
                for (b = 0; b < clcn * layer1_size; b++)
                    cent[b] = 0;

                for (b = 0; b < clcn; b++)
                    centcn[b] = 1;

                for (c = 0; c < voc->vocab_size; c++)
                {

                    for (d = 0; d < layer1_size; d++)
                        cent[layer1_size * cl[c] + d] += syn0[c * layer1_size + d];

                    centcn[cl[c]]++;
                }

                for (b = 0; b < clcn; b++)
                {
                    closev = 0;

                    for (c = 0; c < layer1_size; c++)
                    {
                        cent[layer1_size * b + c] /= centcn[b];
                        closev += cent[layer1_size * b + c] * cent[layer1_size * b + c];
                    }

                    closev = sqrt(closev);
                    for (c = 0; c < layer1_size; c++)
                        cent[layer1_size * b + c] /= closev;
                }

                for (c = 0; c < voc->vocab_size; c++)
                {
                    closev = -10;
                    closeid = 0;
                    for (d = 0; d < clcn; d++) {
                        x = 0;
                        for (b = 0; b < layer1_size; b++)
                            x += cent[layer1_size * d + b] * syn0[c * layer1_size + b];

                        if (x > closev) {
                            closev = x;
                            closeid = d;
                        }
                    }
                    cl[c] = closeid;
                }
            }
            // Save the K-means classes

            for (a = 0; a < voc->vocab_size; a++)
            {
                fprintf(fo, "%s %d ", voc->vocab[a].word, cl[a]);

                for (b = 0; b < layer1_size; b++){
                    fprintf(fo, "%lf ", syn0[a * layer1_size + b]);
                }
                fprintf(fo, "\n");
            }

            free(centcn);
            free(cent);
            free(cl);
            
        }

        fclose(fo);
        free(pt);
    }
}

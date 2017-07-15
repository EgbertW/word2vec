#include "wordmodel.ih"

#include <memory>
#include <fstream>
#include <iostream>

using namespace std;

// paramstruct
//                 voc,
//                 syn0,
//                 syn1,
//                 syn1neg,
//                 expTable,
//                 (&alpha),
//                 starting_alpha,
//                 sample,
//                 (&word_count_actual),
//                 table,
//                 a,
//                 num_threads,
//                 file_size,
//                 MAX_STRING,
//                 EXP_TABLE_SIZE,
//                 0,
//                 layer1_size,
//                 window,
//                 MAX_EXP,
//                 hs,
//                 negative,
//                 table_size,
//                 0,
//                 0,
//                 0,
//                 train_file
//             };


namespace Word2Vec
{
    void WordModel::train(Vocabulary &voc)
    {
        d_params.starting_alpha = alpha;
        initNet();

        if (d_params.negative > 0)
            initUnigramTable();

        start = clock();

        vector<TrainingThread> threads;

        for (size_t a = 0; a < num_threads; ++a)
        {
            Parameters params = d_params;
            if (cbow)
                params.train_type = TrainType::CBOW;
            else
                params.train_type = TrainType::SKIP;

            params.threadNumber = a;
            threads.push_back(shared_ptr<TrainingThread>(new TrainingThread(params)));
        }

        for (auto thread : threads)
            thread.join();

        if (debug_mode > 0)
            out << "Training Ended !\n";

        ofstream output(output_file, ios_base::out | ios_base::binary);

        if (d_params.classes == 0)
        {
            // Save the word vectors
            output << d_params.vocabulary->size() << ' ' << d_params.layer1_size << '\n';
            for (size_t a = 0; a < d_params.vocabulary->size(); ++a)
            {
                output << d_params.vocabulary->get(a).word() << ' ';

                if (binary)
                {
                    for (size_t b = 0; b < layer1_size; ++b)
                        output.write(reinterpret_cast<char *>(
                            &(d_params.syn0[a * d_params.layer1_size + b]),
                            sizeof(real)
                        );
                }
                else
                {
                    for (size_t b = 0; b < layer1_size; ++b)
                        output << syn0[a * d_params.layer1_size + b] << ' ';
                }

                output << endl;
            }
        }
        else
        {
            // Run K-means on the word vectors
            int clcn = classes, iter = 10, closeid;
            int *cl = new int[d_params.vocabulary->size()];
            int *centcn = new int[classes];
            real *cent = new real[classes * d_params.layer1_size];

            for (size_t a = 0; a < d_params.vocabulary->size(); ++a)
                cl[a] = a % clcn;

            for (size_t a = 0; a < iter; ++a)
            {
                for (size_t b = 0; b < clcn * d_params.layer1_size; ++b)
                    cent[b] = 0;

                for (size_t b = 0; b < clcn; ++b)
                    centcn[b] = 1;

                for (size_t c = 0; c < d_params.vocabulary->size(); ++c)
                {
                    for (size_t d = 0; d < d_params.layer1_size; ++d)
                        cent[d_params.layer1_size * cl[c] + d] += d_params.syn0[c * d_params.layer1_size + d];

                    centcn[cl[c]]++;
                }

                for (size_t b = 0; b < clcn; ++b)
                {
                    closev = 0;

                    for (size_t c = 0; c < d_params.layer1_size; ++c)
                    {
                        cent[layer1_size * b + c] /= centcn[b];
                        closev += cent[d_params.layer1_size * b + c] * cent[d_params.layer1_size * b + c];
                    }

                    real closev = sqrt(closev);
                    for (size_t c = 0; c < layer1_size; ++c)
                        cent[d_params.layer1_size * b + c] /= closev;
                }

                for (size_t c = 0; c < d_params.vocabulary->size(); ++c)
                {
                    real closev = -10;
                    int closeid = 0;
                    for (size_t d = 0; d < clcn; ++d)
                    {
                        real x = 0;
                        for (size_t b = 0; b < d_params.layer1_size; ++b)
                            x += cent[d_params.layer1_size * d + b] * d_params.syn0[c * d_params.layer1_size + b];

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
                output << d_params.vocubulary->get(a).word() << ' ' << cl[a] << ' ';

                for (size_t b = 0; b < d_params.layer1_size; ++b)
                    output << d_params.syn0[a * d_params.layer1_size + b];
                    
                output << endl;
            }

            delete [] centcn;
            delete [] cent;
            delete [] cl;
        }

        output.close();
    }
}

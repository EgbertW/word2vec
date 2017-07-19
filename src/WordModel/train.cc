#include "wordmodel.ih"

#include <memory>
#include <iostream>
#include <cmath>
#include <vector>

#include <boost/format.hpp>

#include <libword2vec/TrainingThread.h>

using namespace std;

namespace Word2Vec
{
    void WordModel::train()
    {
        d_params.starting_alpha = *d_params.alpha;
        vector<real> &expTable = *d_params.expTable;

        if (d_params.negative > 0)
            initUnigramTable();

        d_params.start = clock();

        vector<shared_ptr<TrainingThread>> threads;

        for (size_t a = 0; a < d_params.num_threads; ++a)
        {
            Parameters params = d_params;
            threads.push_back(shared_ptr<TrainingThread>(new TrainingThread(a, params)));
        }

        for (auto ptr = threads.begin(); ptr != threads.end(); ++ptr)
            (*ptr)->join();

        if (d_params.debug_mode > 0)
            cout << "Training Ended !\n";
    }
}

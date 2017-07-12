namespace Word2Vec
{
    void WordModel::~WordModel()
    {
        if (syn0 != nullptr)
            free(syn0);

        if (syn1 != nullptr)
            free(syn1);

        if (syn1neg != nullptr)
          free(syn1neg);
    }
}

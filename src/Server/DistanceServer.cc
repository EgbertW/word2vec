//
// HttpServer.cc
// ~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <libword2vec/Server/Server.h>

#include <libword2vec/Parameters.h>
#include <libword2vec/WordModel.h>

using namespace Word2Vec;
using namespace std;

typedef WordModel::WordResult WordResult;

int main(int argc, char* argv[])
{
    try
    {
        // Check command line arguments.
        if (argc != 4)
        {
            std::cerr << "Usage: HttpServer <address> <port> <vector-file>\n";
            return 1;
        }

        string file_name(argv[3]);
        Parameters params;
        params.debug_mode = 2;
        WordModel model(params);
        model.readWordModels(file_name);
        
        // Initialise the server.
        Word2Vec::Server::Server s(argv[1], argv[2], model);
        
        // Run the server until stopped.
        s.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }
    
    return 0;
}

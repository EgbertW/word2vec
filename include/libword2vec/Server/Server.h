//
// Server.h
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WORD2VEC_SERVER_SERVER_H
#define WORD2VEC_SERVER_SERVER_H

#include <boost/asio.hpp>
#include <string>
#include "Connection.h"
#include "ConnectionManager.h"
#include "RequestHandler.h"

namespace Word2Vec {
namespace Server {

/// The top-level class of the HTTP server.
class Server
{
    public:
        Server(Server const &) = delete;
        Server &operator=(Server const &) = delete;
        
        /// Construct the server to listen on the specified TCP address and port, and
        /// serve up files from the given directory.
        explicit Server(
            std::string const &address,
            std::string const &port,
            std::string const &doc_root
        );
        
        /// Run the server's io_service loop.
        void run();
    
    private:
        /// Perform an asynchronous accept operation.
        void doAccept();
        
        /// Wait for a request to stop the server.
        void doAwaitStop();
        
        /// The io_service used to perform asynchronous operations.
        boost::asio::io_service d_io_service;
        
        /// The signal_set is used to register for process termination notifications.
        boost::asio::signal_set d_signals;
        
        /// Acceptor used to listen for incoming connections.
        boost::asio::ip::tcp::acceptor d_acceptor;
        
        /// The connection manager which owns all live connections.
        ConnectionManager d_connection_manager;
        
        /// The next socket to be accepted.
        boost::asio::ip::tcp::socket d_socket;
        
        /// The handler for all incoming requests.
        RequestHandler d_request_handler;
};

} // namespace Server
} // namespace Word2Vec

#endif // WORD2VEC_SERVER_SERVER_HPP

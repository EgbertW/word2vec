//
// Connection.h
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WORD2VEC_SERVER_CONNECTION_H
#define WORD2VEC_SERVER_CONNECTION_H

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include "Reply.h"
#include "Request.h"
#include "RequestHandler.h"
#include "RequestParser.h"

namespace Word2Vec {
namespace Server {

class ConnectionManager;

/// Represents a single connection from a client.
class Connection
  : public std::enable_shared_from_this<Connection>
{
    public:
        typedef std::shared_ptr<Connection> Ptr;

        Connection(Connection const &) = delete;
        Connection& operator=(Connection const &) = delete;

        /// Construct a connection with the given socket.
        explicit Connection(
            boost::asio::ip::tcp::socket socket,
            ConnectionManager &manager, 
            RequestHandler &handler
        );

        /// Start the first asynchronous operation for the connection.
        void start();

        /// Stop all asynchronous operations associated with the connection.
        void stop();

    private:
        /// Perform an asynchronous read operation.
        void doRead();

        /// Perform an asynchronous write operation.
        void doWrite();

        /// Socket for the connection.
        boost::asio::ip::tcp::socket d_socket;

        /// The manager for this connection.
        ConnectionManager &d_connection_manager;

        /// The handler used to process the incoming request.
        RequestHandler &d_request_handler;

        /// Buffer for incoming data.
        std::array<char, 8192> d_buffer;

        /// The incoming request.
        Request d_request;

        /// The parser for the incoming request.
        RequestParser d_request_parser;

        /// The reply to be sent back to the client.
        Reply d_reply;
};

} // namespace Server
} // namespace Word2Vec

#endif // WORD2VEC_SERVER_CONNECTION_H

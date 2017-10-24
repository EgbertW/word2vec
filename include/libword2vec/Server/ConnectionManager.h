//
// ConnectionManager.h
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WORD2VEC_SERVER_CONNECTION_MANAGER_H
#define WORD2VEC_SERVER_CONNECTION_MANAGER_H

#include <set>
#include "Connection.h"

namespace Word2Vec {
namespace Server {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class ConnectionManager
{
    public:
        ConnectionManager(ConnectionManager const &) = delete;
        ConnectionManager &operator=(ConnectionManager const &) = delete;
        
        /// Construct a connection manager.
        ConnectionManager();
        
        /// Add the specified connection to the manager and start it.
        void start(Connection::Ptr c);
        
        /// Stop the specified connection.
        void stop(Connection::Ptr c);
        
        /// Stop all connections.
        void stopAll();
    
    private:
        /// The managed connections.
        std::set<Connection::Ptr> d_connections;
};

} // namespace Server
} // namespace Word2Vec

#endif // WORD2VEC_SERVER_CONNECTION_MANAGER_H

//
// RequestHandler.h
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WORD2VEC_SERVER_REQUEST_HANDLER_H
#define WORD2VEC_SERVER_REQUEST_HANDLER_H

#include <string>
#include <libword2vec/WordModel.h>

namespace Word2Vec {
namespace Server {

struct Reply;
struct Request;

/// The common handler for all incoming requests.
class RequestHandler
{
    public:
        RequestHandler(RequestHandler const &) = delete;
        RequestHandler& operator=(RequestHandler const &) = delete;
        
        /// Construct with a directory containing files to be served.
        explicit RequestHandler(Word2Vec::WordModel &model);
        
        /// Handle a request and produce a reply.
        void handleRequest(Request const &req, Reply &rep);
    
    private:
        /// The directory containing the files to be served.
        Word2Vec::WordModel &d_word_model;
        
        /// Perform URL-decoding on a string. Returns false if the encoding was
        /// invalid.
        static bool url_decode(const std::string& in, std::string& out);
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP

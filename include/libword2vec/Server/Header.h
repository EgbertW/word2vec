//
// Header.h
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WORD2VEC_SERVER_HEADER_H
#define WORD2VEC_SERVER_HEADER_H

#include <string>

namespace Word2Vec {
namespace Server {

struct Header
{
    std::string name;
    std::string value;
};

} // namespace server
} // namespace http

#endif // WORD2VEC_SERVER_HEADER_H

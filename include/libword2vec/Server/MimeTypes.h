//
// MimeTypes.h
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WORD2VEC_SERVER_MIME_TYPES_H
#define WORD2VEC_SERVER_MIME_TYPES_H

#include <string>

namespace Word2Vec {
namespace Server {
namespace MimeTypes {

/// Convert a file extension into a MIME type.
std::string extension_to_type(std::string const &extension);

} // namespace MimeTypes
} // namespace Server
} // namespace Word2Vec

#endif // WORD2VEC_SERVER_MIME_TYPES_H

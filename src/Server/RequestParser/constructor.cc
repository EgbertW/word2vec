#include <libword2vec/Server/RequestParser.h>
#include <libword2vec/Server/Request.h>

namespace Word2Vec {
namespace Server {

RequestParser::RequestParser()
:
    d_state(method_start)
{}

} // namespace Server
} // namespace Word2Vec

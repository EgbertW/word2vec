#include "RequestHandler.ih"

namespace Word2Vec {
namespace Server {

RequestHandler::RequestHandler(std::string const &doc_root)
:
    d_doc_root(doc_root)
{}

} // namespace Server
} // namespace Word2Vec

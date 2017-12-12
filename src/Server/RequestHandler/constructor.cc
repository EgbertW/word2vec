#include "RequestHandler.ih"

namespace Word2Vec {
namespace Server {

RequestHandler::RequestHandler(Word2Vec::WordModel &model)
:
    d_word_model(model)
{}

} // namespace Server
} // namespace Word2Vec

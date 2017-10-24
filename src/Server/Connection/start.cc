#include "Connection.ih"

namespace Word2Vec {
namespace Server {

void Connection::start()
{
    doRead();
}

} // namespace Server
} // namespace Word2Vec

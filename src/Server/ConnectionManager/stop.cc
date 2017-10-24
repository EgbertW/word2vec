#include <libword2vec/Server/ConnectionManager.h>

namespace Word2Vec {
namespace Server {

void ConnectionManager::stop(Connection::Ptr c)
{
    d_connections.erase(c);
    c->stop();
}

} // namespace Server
} // namespace Word2Vec

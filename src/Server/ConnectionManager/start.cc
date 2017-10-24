#include <libword2vec/Server/ConnectionManager.h>

namespace Word2Vec {
namespace Server {

void ConnectionManager::start(Connection::Ptr c)
{
    d_connections.insert(c);
    c->start();
}

} // namespace Server
} // namespace Word2Vec

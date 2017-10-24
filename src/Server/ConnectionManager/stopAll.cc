#include <libword2vec/Server/ConnectionManager.h>

namespace Word2Vec {
namespace Server {

void ConnectionManager::stopAll()
{
    for (auto c: d_connections)
        c->stop();
    d_connections.clear();
}

} // namespace Server
} // namespace Word2Vec

#include "Connection.ih"

namespace Word2Vec {
namespace Server {

void Connection::stop()
{
    d_socket.close();
}

} // namespace Server
} // namespace Word2Vec

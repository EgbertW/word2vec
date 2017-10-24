#include <libword2vec/Server/Server.h>
#include <signal.h>
#include <utility>

namespace Word2Vec {
namespace Server {

void Server::run()
{
    // The io_service::run() call will block until all asynchronous operations
    // have finished. While the server is running, there is always at least one
    // asynchronous operation outstanding: the asynchronous accept call waiting
    // for new incoming connections.
    d_io_service.run();
}

} // namespace Server
} // namespace Word2Vec

#include <libword2vec/Server/Server.h>
#include <signal.h>
#include <utility>
#include <iostream>

namespace Word2Vec {
namespace Server {

void Server::run()
{
    // The io_service::run() call will block until all asynchronous operations
    // have finished. While the server is running, there is always at least one
    // asynchronous operation outstanding: the asynchronous accept call waiting
    // for new incoming connections.
    std::cout << "Ready to accept connections\n";
    d_io_service.run();
}

} // namespace Server
} // namespace Word2Vec

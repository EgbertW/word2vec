#include <libword2vec/Server/Server.h>
#include <signal.h>
#include <utility>

namespace Word2Vec {
namespace Server {

void Server::doAwaitStop()
{
    d_signals.async_wait(
        [this](boost::system::error_code /*ec*/, int /*signo*/)
        {
            // The server is stopped by cancelling all outstanding asynchronous
            // operations. Once all operations have finished the io_service::run()
            // call will exit.
            d_acceptor.close();
            d_connection_manager.stopAll();
        }
    );
}

} // namespace Server
} // namespace Word2Vec

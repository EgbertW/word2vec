#include <libword2vec/Server/Server.h>
#include <signal.h>
#include <utility>

namespace Word2Vec {
namespace Server {

void Server::doAccept()
{
    d_acceptor.async_accept(
        d_socket,
        [this](boost::system::error_code ec)
        {
            // Check whether the server was stopped by a signal before this
            // completion handler had a chance to run.
            if (!d_acceptor.is_open())
            {
                return;
            }
    
            if (!ec)
            {
                d_connection_manager.start(
                    std::make_shared<Connection>(
                        std::move(d_socket), d_connection_manager, d_request_handler
                    )
                );
            }
    
            doAccept();
        });
}

} // namespace Server
} // namespace Word2Vec

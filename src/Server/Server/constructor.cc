#include <libword2vec/Server/Server.h>
#include <signal.h>
#include <utility>

namespace Word2Vec {
namespace Server {

Server::Server(
    const std::string& address,
    const std::string& port,
    Word2Vec::WordModel &model
)
:
    d_io_service(),
    d_signals(d_io_service),
    d_acceptor(d_io_service),
    d_connection_manager(),
    d_socket(d_io_service),
    d_request_handler(model)
{
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    d_signals.add(SIGINT);
    d_signals.add(SIGTERM);
#if defined(SIGQUIT)
    d_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)

      doAwaitStop();

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver resolver(d_io_service);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
    d_acceptor.open(endpoint.protocol());
    d_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    d_acceptor.bind(endpoint);
    d_acceptor.listen();
    
    doAccept();
}

} // namespace Server
} // namespace Word2Vec

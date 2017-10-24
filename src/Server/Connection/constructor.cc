#include "Connection.ih"

namespace Word2Vec {
namespace Server {

Connection::Connection(boost::asio::ip::tcp::socket socket, ConnectionManager &manager, RequestHandler &handler)
: 
    d_socket(std::move(socket)),
    d_connection_manager(manager),
    d_request_handler(handler)
{}

} // namespace Server
} // namespace Word2Vec

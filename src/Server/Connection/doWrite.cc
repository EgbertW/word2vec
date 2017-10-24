#include "Connection.ih"

namespace Word2Vec {
namespace Server {

void Connection::doWrite()
{
    auto self(shared_from_this());
    boost::asio::async_write(d_socket, d_reply.toBuffers(),
        [this, self](boost::system::error_code ec, std::size_t)
        {
            if (!ec)
            {
                // Initiate graceful connection closure.
                boost::system::error_code ignored_ec;
                d_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }

            if (ec != boost::asio::error::operation_aborted)
                d_connection_manager.stop(shared_from_this());
        }
    );
}

} // namespace Server
} // namespace Word2Vec

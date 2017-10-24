#include "Connection.ih"

namespace Word2Vec {
namespace Server {

void Connection::doRead()
{
    auto self(shared_from_this());
    d_socket.async_read_some(boost::asio::buffer(d_buffer),
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
                RequestParser::ResultType result;
                std::tie(result, std::ignore) = d_request_parser.parse(
                    d_request, d_buffer.data(), d_buffer.data() + bytes_transferred
                );
    
                if (result == RequestParser::good)
                {
                    d_request_handler.handleRequest(d_request, d_reply);
                    doWrite();
                }
                else if (result == RequestParser::bad)
                {
                    d_reply = Reply::stock_reply(Reply::bad_request);
                    doWrite();
                }
                else
                {
                    doRead();
                }
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
              d_connection_manager.stop(shared_from_this());
            }
        });
}

} // namespace Server
} // namespace Word2Vec

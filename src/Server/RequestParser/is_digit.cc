#include <libword2vec/Server/RequestParser.h>
#include <libword2vec/Server/Request.h>

namespace Word2Vec {
namespace Server {

bool RequestParser::is_digit(int c)
{
    return c >= '0' && c <= '9';
}

} // namespace Server
} // namespace Word2Vec

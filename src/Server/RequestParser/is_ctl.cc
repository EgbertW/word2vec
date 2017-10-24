#include <libword2vec/Server/RequestParser.h>
#include <libword2vec/Server/Request.h>

namespace Word2Vec {
namespace Server {

bool RequestParser::is_ctl(int c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

} // namespace Server
} // namespace Word2Vec

#include <libword2vec/Server/RequestParser.h>
#include <libword2vec/Server/Request.h>

namespace Word2Vec {
namespace Server {

bool RequestParser::is_tspecial(int c)
{
    switch (c)
    {
        case '(': case ')': case '<': case '>': case '@':
        case ',': case ';': case ':': case '\\': case '"':
        case '/': case '[': case ']': case '?': case '=':
        case '{': case '}': case ' ': case '\t':
            return true;
        default:
            return false;
    }
}

} // namespace Server
} // namespace Word2Vec

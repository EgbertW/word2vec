#include <libword2vec/Server/MimeTypes.h>

namespace Word2Vec {
namespace Server {
namespace MimeTypes {

struct Mapping
{
    char const *extension;
    char const *mime_type;
} mappings[] =
{
  { "gif", "image/gif" },
  { "htm", "text/html" },
  { "html", "text/html" },
  { "jpg", "image/jpeg" },
  { "png", "image/png" }
};

std::string extension_to_type(std::string const &extension)
{
    for (Mapping m: mappings)
    {
        if (m.extension == extension)
        {
            return m.mime_type;
        }
    }
    
    return "text/plain";
}

} // namespace MimeTypes
} // namespace Server
} // namespace Word2Vec

#include "RequestHandler.ih"
#include <iostream>
#include <map>

namespace Word2Vec {
namespace Server {

// void fn()
// {
//     while (true)
//     {
//         printf("Enter word or sentence (EXIT to break): ");
// 
//         string line;
//         getline(cin, line);
// 
//         if (line == "EXIT")
//             break;
// 
//         vector<WordResult> results = model.findWords(line, NUM_WORDS);
// 
//         if (results.empty())
//             continue;
// 
//         cout << "\n                                              Word       Cosine distance\n"
//             "------------------------------------------------------------------------\n";
// 
//         for (WordResult const &r : results)
//             cout << (boost::format("%50s\t\t%f\n") % r.second % r.first);
//     }
// }

void RequestHandler::handleRequest(Request const &req, Reply &rep)
{
    // Decode url to path.
    std::string request_path;
    if (!url_decode(req.uri, request_path))
    {
        rep = Reply::stock_reply(Reply::bad_request);
        return;
    }
    
    // Request path must be absolute and not contain "..".
    if (request_path.empty() || request_path[0] != '/' || request_path.find("..") != std::string::npos)
    {
        rep = Reply::stock_reply(Reply::bad_request);
        std::cout << "[400] GET " << request_path << std::endl;
        return;
    }

    // Find out if there is a query
    size_t query_pos = request_path.find("?");
    std::map<std::string, std::string> query;
    if (query_pos != std::string::npos)
    {
        std::string query_str = request_path.substr(query_pos + 1);
        request_path = request_path.substr(0, query_pos);

        while (not query_str.empty())
        {
            query_pos = query_str.find("&");
            std::string param;
            if (query_pos != std::string::npos)
            {
                param = query_str.substr(0, query_pos);
                query_str = query_str.substr(query_pos + 1);
            }
            else
            {
                param = query_str;
                query_str = "";
            }

            if (not param.empty())
            {
                if ((query_pos = param.find("=")) != std::string::npos)
                {
                    std::string key = param.substr(0, query_pos);
                    std::string value = param.substr(query_pos + 1);
                    query[key] = value;
                }
                else
                {
                    query[param] = "1";
                }
            }
        }
    }

    if (query.find("word") != query.end())
    {
        std::string word = query["word"];
        std::cout << "Looking for word " << word << std::endl;
    }
    
    // If path ends in slash (i.e. is a directory) then add "index.html".
    if (request_path[request_path.size() - 1] == '/')
    {
        request_path += "index.html";
    }
    
    // Determine the file extension.
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;

    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
    {
        extension = request_path.substr(last_dot_pos + 1);
    }
    
    // Open the file to send back.
    std::string full_path = "foo/" + request_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    if (!is)
    {
        rep = Reply::stock_reply(Reply::not_found);
        std::cout << "[404] GET " << request_path << std::endl;
        return;
    }
    
    // Fill out the reply to be sent to the client.
    std::cout << "[200] GET " << request_path << std::endl;
    rep.status = Reply::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
      rep.content.append(buf, is.gcount());
    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = MimeTypes::extension_to_type(extension);
}

} // namespace Server
} // namespace Word2Vec

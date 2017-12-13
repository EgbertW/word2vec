#include "RequestHandler.ih"
#include <iostream>
#include <map>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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
using boost::property_tree::ptree;
using Word2Vec::WordModel; 
using namespace std;


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
        std::cout << "[400] " << req.method << " " << request_path << std::endl;
        return;
    }

    // Find out if there is a query
    size_t query_pos = request_path.find("?");
    //std::map<std::string, std::string> query;

    ptree query;
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
                    size_t brackets = key.rfind("[]");
                    if (string::npos != brackets)
                        key = key.substr(0, brackets);
                    query.add(key, value);
                }
                else
                {
                    query.add(param, "1");
                }
            }
        }
    }

    std::ostringstream q_json;
    boost::property_tree::write_json(q_json, query, false);
    cout << "Parsed query: " << q_json.str()<< endl;

    if ("/vector" == request_path && "GET" == req.method)
    {
        auto iters = query.equal_range("token");
        auto start = iters.first;
        auto end = iters.second;

        ptree response;
        ptree vectors;

        Vocabulary const &vocab(d_word_model.vocab());
        for ( ; start != end; ++start)
        {
            string token = start->second.get_value<string>();
            vector<WordModel::real> vec(d_word_model.getVector(token));
            //size_t pos = vocab.search(token);

            ptree tree_vec;
            ptree elem;
            for (WordModel::real r : vec)
            {
                elem.put_value(r);
                tree_vec.push_back(std::make_pair("", elem));
            }

            vectors.add_child(token, tree_vec);
        }
        response.put_child("vectors", vectors);

        ostringstream json;
        boost::property_tree::write_json(json, response);

        rep.status = Reply::ok;
        rep.headers.resize(1);
        rep.headers[0].name = "Content-Type";
        rep.headers[0].value = "application/json";
        rep.content.append(json.str());
        std::cout << "[200] " << req.method << " /vector" << std::endl;
        return;
    }

    if (query.count("word") > 0)
    {
        std::string word = query.get_value<string>("word");
        std::cout << "Looking for word " << word << std::endl;
        std::vector<WordModel::WordResult> results = d_word_model.findWords(word);

        ptree tree;
        ptree results_tree;

        for (WordModel::WordResult r : results) {
            ptree node;
            node.put("word", r.second);
            node.put("score", r.first);
            results_tree.push_back(std::make_pair("", node));
        }

        tree.add_child("results", results_tree);

        std::ostringstream json;
        boost::property_tree::write_json(json, tree, false);

        rep.status = Reply::ok;
        rep.headers.resize(1);
        rep.headers[0].name = "Content-Type";
        rep.headers[0].value = "application/json";

        rep.content.append(json.str());
        std::cout << "[200] " << req.method << " word=" << word << std::endl;
        return;
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
        std::cout << "[404] " << req.method << " " << request_path << std::endl;
        return;
    }
    
    // Fill out the reply to be sent to the client.
    std::cout << "[200] " << req.method << " " << request_path << std::endl;
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

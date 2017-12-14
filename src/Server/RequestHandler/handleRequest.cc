#include "RequestHandler.ih"
#include <iostream>
#include <map>
#include <sstream>
#include <set>

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
    string request_path;
    if (!url_decode(req.uri, request_path))
    {
        rep = Reply::stock_reply(Reply::bad_request);
        return;
    }
    
    // Request path must be absolute and not contain "..".
    if (request_path.empty() || request_path[0] != '/' || request_path.find("..") != string::npos)
    {
        rep = Reply::stock_reply(Reply::bad_request);
        cout << "[400] " << req.method << " " << request_path << endl;
        return;
    }

    // Find out if there is a query
    size_t query_pos = request_path.find("?");
    //map<string, string> query;

    ptree query;
    if (query_pos != string::npos)
    {
        string query_str = request_path.substr(query_pos + 1);
        request_path = request_path.substr(0, query_pos);

        while (not query_str.empty())
        {
            query_pos = query_str.find("&");
            string param;
            if (query_pos != string::npos)
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
                if ((query_pos = param.find("=")) != string::npos)
                {
                    string key = param.substr(0, query_pos);
                    string value = param.substr(query_pos + 1);
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

    ostringstream q_json;
    boost::property_tree::write_json(q_json, query, false);
    cout << "Parsed query: " << q_json.str()<< endl;

    if (request_path.find("/vector") == 0)
    {
        set<string> tokens;
        if ("GET" == req.method)
        {
            auto iters = query.equal_range("token");
            auto start = iters.first;
            auto end = iters.second;

            for ( ; start != end; ++start)
            {
                string token = start->second.get_value<string>();
                tokens.insert(token);
            }
        }
        else if ("POST" == req.method)
        {
            bool req_is_json = false;
            bool req_is_text = false;
            string encoding = "utf-8";
            for (Header h : req.headers)
            {
                if ("Content-Type" == h.name && "application/json" == h.value)
                {
                    req_is_json = true;
                }
                else if ("Content-Type" == h.name && h.value.find("text/plain") == 0)
                {
                    req_is_text = true;
                    size_t charset_pos = h.value.find("charset");
                    if (charset_pos != string::npos)
                        encoding = h.value.substr(charset_pos + 8);
                }
            }

            if (req_is_json)
            {
                istringstream json_str(req.payload);
                ptree req_data;
                boost::property_tree::read_json(json_str, req_data);

                ptree req_tokens = req_data.get_child("token");
                for (auto ptr = req_tokens.begin(); ptr != req_tokens.end(); ++ptr)
                    tokens.insert(ptr->second.get_value<string>());

                string self_val = req_tokens.get_value<string>();
                if (self_val.size())
                    tokens.insert(self_val);
            }
            else if (req_is_text)
            {
                if ("utf-8" != encoding)
                {
                    rep = Reply::stock_reply(Reply::bad_request);
                    cout << "[400] " << req.method << " " << request_path << " -> Bad Encoding" << endl;
                    return;
                }

                // Tokenize on whitespace
                string buf;
                for (char ch : req.payload)
                {
                    if (' ' == ch || '\n' == ch || '\t' == ch || '\r' == ch)
                    {
                        if (buf.size() > 0)
                            tokens.insert(buf);
                        buf = "";
                    }
                    else
                    {
                        buf.push_back(ch);
                    }
                }
                if (buf.size() > 0)
                    tokens.insert(buf);
            }
            else
            {
                rep = Reply::stock_reply(Reply::bad_request);
                cout << "[400] " << req.method << " " << request_path << endl;
                return;
            }
        }

        ptree response;
        ptree vectors;

        Vocabulary const &vocab(d_word_model.vocab());
        for (string token : tokens)
        {
            vector<WordModel::real> vec(d_word_model.getVector(token));
            size_t pos = vocab.search(token);

            ptree tree_vec;
            ptree elem;
            for (WordModel::real r : vec)
            {
                elem.put_value(r);
                tree_vec.push_back(make_pair("", elem));
            }

            ptree token_data;
            token_data.put("position", pos);
            token_data.put_child("vector", tree_vec);

            vectors.add_child(token, token_data);
        }
        response.put_child("tokens", vectors);

        ostringstream json;
        boost::property_tree::write_json(json, response);

        rep.status = Reply::ok;
        rep.headers.resize(1);
        rep.headers[0].name = "Content-Type";
        rep.headers[0].value = "application/json";
        rep.content.append(json.str());
        cout << "[200] " << req.method << " /vector" << endl;
        return;
    }

    if (query.count("word") > 0)
    {
        string word = query.get_value<string>("word");
        cout << "Looking for word " << word << endl;
        vector<WordModel::WordResult> results = d_word_model.findWords(word);

        ptree tree;
        ptree results_tree;

        for (WordModel::WordResult r : results) {
            ptree node;
            node.put("word", r.second);
            node.put("score", r.first);
            results_tree.push_back(make_pair("", node));
        }

        tree.add_child("results", results_tree);

        ostringstream json;
        boost::property_tree::write_json(json, tree, false);

        rep.status = Reply::ok;
        rep.headers.resize(1);
        rep.headers[0].name = "Content-Type";
        rep.headers[0].value = "application/json";

        rep.content.append(json.str());
        cout << "[200] " << req.method << " word=" << word << endl;
        return;
    }
    
    // If path ends in slash (i.e. is a directory) then add "index.html".
    if (request_path[request_path.size() - 1] == '/')
    {
        request_path += "index.html";
    }
    
    // Determine the file extension.
    size_t last_slash_pos = request_path.find_last_of("/");
    size_t last_dot_pos = request_path.find_last_of(".");
    string extension;

    if (last_dot_pos != string::npos && last_dot_pos > last_slash_pos)
    {
        extension = request_path.substr(last_dot_pos + 1);
    }
    
    // Open the file to send back.
    string full_path = "foo/" + request_path;
    ifstream is(full_path.c_str(), ios::in | ios::binary);
    if (!is)
    {
        rep = Reply::stock_reply(Reply::not_found);
        cout << "[404] " << req.method << " " << request_path << endl;
        return;
    }
    
    // Fill out the reply to be sent to the client.
    cout << "[200] " << req.method << " " << request_path << endl;
    rep.status = Reply::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
      rep.content.append(buf, is.gcount());
    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = MimeTypes::extension_to_type(extension);
}

} // namespace Server
} // namespace Word2Vec

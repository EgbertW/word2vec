#include <libword2vec/Server/RequestParser.h>
#include <libword2vec/Server/Request.h>

#include <iostream>
#include <algorithm>

namespace Word2Vec {
namespace Server {

RequestParser::ResultType RequestParser::consume(Request &req, char input)
{
    std::cout << input << std::flush;
    switch (d_state)
    {
        case method_start:
            if (!is_char(input) || is_ctl(input) || is_tspecial(input))
            {
                return bad;
            }
            else
            {
                d_state = method;
                req.method.push_back(input);
                return indeterminate;
            }
        case method:
            if (input == ' ')
            {
                d_state = uri;
                return indeterminate;
            }
            else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
            {
                return bad;
            }
            else
            {
                req.method.push_back(input);
                return indeterminate;
            }
        case uri:
            if (input == ' ')
            {
                d_state = http_version_h;
                return indeterminate;
            }
            else if (is_ctl(input))
            {
                return bad;
            }
            else
            {
                req.uri.push_back(input);
                return indeterminate;
            }
        case http_version_h:
            if (input == 'H')
            {
                d_state = http_version_t_1;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case http_version_t_1:
            if (input == 'T')
            {
                d_state = http_version_t_2;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case http_version_t_2:
            if (input == 'T')
            {
                d_state = http_version_p;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case http_version_p:
            if (input == 'P')
            {
                d_state = http_version_slash;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case http_version_slash:
            if (input == '/')
            {
                req.http_version_major = 0;
                req.http_version_minor = 0;
                d_state = http_version_major_start;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case http_version_major_start:
            if (is_digit(input))
            {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                d_state = http_version_major;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case http_version_major:
            if (input == '.')
            {
                d_state = http_version_minor_start;
                return indeterminate;
            }
            else if (is_digit(input))
            {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case http_version_minor_start:
            if (is_digit(input))
            {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                d_state = http_version_minor;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case http_version_minor:
            if (input == '\r')
            {
                d_state = expecting_newline_1;
                return indeterminate;
            }
            else if (is_digit(input))
            {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case expecting_newline_1:
            if (input == '\n')
            {
                d_state = header_line_start;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case header_line_start:
            if (input == '\r')
            {
                d_state = expecting_newline_3;
                return indeterminate;
            }
            else if (!req.headers.empty() && (input == ' ' || input == '\t'))
            {
                d_state = header_lws;
                return indeterminate;
            }
            else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
            {
                return bad;
            }
            else
            {
                if (req.headers.size() >= MAX_HEADERS)
                {
                    std::cerr << "ERROR: Too many header - > " << MAX_HEADERS << std::endl;
                    return bad;
                }
                req.headers.push_back(Header());
                req.headers.back().name.push_back(input);
                d_state = header_name;
                return indeterminate;
            }
        case header_lws:
            if (input == '\r')
            {
                d_state = expecting_newline_2;
                return indeterminate;
            }
            else if (input == ' ' || input == '\t')
            {
                return indeterminate;
            }
            else if (is_ctl(input))
            {
                return bad;
            }
            else
            {
                d_state = header_value;
                req.headers.back().value.push_back(input);
                return indeterminate;
            }
        case header_name:
            if (input == ':')
            {
                d_state = space_before_header_value;
                return indeterminate;
            }
            else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
            {
                return bad;
            }
            else
            {
                if (req.headers.back().name.size() >= MAX_HEADER_LENGTH)
                {
                    std::cerr << "ERROR: Too long header name: " << req.headers.back().name << "\n";
                    return bad;
                }

                req.headers.back().name.push_back(input);
                return indeterminate;
            }
        case space_before_header_value:
            if (input == ' ')
            {
                d_state = header_value;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case header_value:
            if (input == '\r')
            {
                d_state = expecting_newline_2;
                return indeterminate;
            }
            else if (is_ctl(input))
            {
                return bad;
            }
            else
            {
                if (req.headers.back().value.size() >= MAX_HEADER_VALUE_LENGTH)
                {
                    std::cerr << "ERROR: Too long header value: " << req.headers.back().name << "\n";
                    return bad;
                }

                req.headers.back().value.push_back(input);
                return indeterminate;
            }
        case expecting_newline_2:
            if (input == '\n')
            {
                d_state = header_line_start;
                return indeterminate;
            }
            else
            {
                return bad;
            }
        case expecting_newline_3:
            if (input != '\n')
                return bad;

            d_remaining_content_size = 0;
            for (Header h : req.headers)
            {
                std::string n = h.name;
                std::transform(n.begin(), n.end(), n.begin(), ::tolower);
                if ("content-length" == n)
                {
                    try
                    {
                        d_remaining_content_size = std::stoi(h.value);
                        std::cout << "Header: " << n << " = " << d_remaining_content_size << std::endl;
                        break;
                    }
                    catch (std::invalid_argument const &e)
                    {
                        std::cerr << "ERROR: Could not convert " << h.value << " to number\n";
                        return bad;
                    }
                    catch (std::out_of_range const &e)
                    {
                        std::cerr << "ERROR: Could not convert " << h.value << " to number\n";
                        return bad;
                    }
                }
            }

            if (d_remaining_content_size > MAX_CONTENT_LENGTH)
            {
                std::cerr << "ERROR: Too large content: " << d_remaining_content_size << std::endl;
                return bad;
            }

            if (0 == d_remaining_content_size)
                return good;

            d_state = expecting_content;
            std::cout << "Expecting " << d_remaining_content_size << " bytes to be read\n";
            return indeterminate;
        case expecting_content:
            req.payload.push_back(input);
            return 0 == --d_remaining_content_size ? good : indeterminate;
        default:
            return bad;
    }
}

} // namespace Server
} // namespace Word2Vec

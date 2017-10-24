#include <libword2vec/Server/RequestParser.h>
#include <libword2vec/Server/Request.h>

namespace Word2Vec {
namespace Server {

RequestParser::ResultType RequestParser::consume(Request &req, char input)
{
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
            return (input == '\n') ? good : bad;
        default:
            return bad;
    }
}

} // namespace Server
} // namespace Word2Vec

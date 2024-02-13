#ifndef GW2COMBAT_SERVER_HTTP_HPP
#define GW2COMBAT_SERVER_HTTP_HPP

#include <boost/beast.hpp>

namespace gw2combat {

namespace http = boost::beast::http;

using http_request = http::request<http::string_body>;
using http_response = http::response<http::string_body>;

const static std::string MIME_TYPE_APPLICATION_JSON = "application/json";
const static std::string MIME_TYPE_TEXT_PLAIN = "text/plain";

inline auto bad_request(const http_request& request, const boost::beast::string_view why)
    -> http_response {
    http_response response{http::status::bad_request, request.version()};
    response.set(http::field::content_type, MIME_TYPE_TEXT_PLAIN);
    response.keep_alive(request.keep_alive());
    response.body() = std::string(why);
    response.prepare_payload();
    return response;
}

inline auto not_found(const http_request& request, const boost::beast::string_view target)
    -> http_response {
    http_response response{http::status::not_found, request.version()};
    response.set(http::field::content_type, MIME_TYPE_TEXT_PLAIN);
    response.keep_alive(request.keep_alive());
    response.body() = "The resource '" + std::string(target) + "' was not found.";
    response.prepare_payload();
    return response;
}

inline auto internal_server_error(const http_request& request, const boost::beast::string_view what)
    -> http_response {
    http_response response{http::status::internal_server_error, request.version()};
    response.set(http::field::content_type, MIME_TYPE_TEXT_PLAIN);
    response.keep_alive(request.keep_alive());
    response.body() = "An error occurred: '" + std::string(what) + "'";
    response.prepare_payload();
    return response;
}

struct http_server_config_t {
    std::string server_host = "127.0.0.1";
    unsigned short server_port = 54321;
    int threads = 1;
};

extern auto start_server_http(const http_server_config_t& config) -> void;

}

#endif  // GW2COMBAT_SERVER_HTTP_HPP

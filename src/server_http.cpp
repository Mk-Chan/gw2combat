#include "server_http.hpp"

#include "boost/asio.hpp"
#include "boost/url.hpp"

#include "spdlog/spdlog.h"

#include "nlohmann/json.hpp"

#include "configuration/encounter.hpp"

#include "combat_loop.hpp"

namespace gw2combat {

using tcp = boost::asio::ip::tcp;

class parsed_request_t {
   public:
    explicit parsed_request_t(const http_request& request)
        : full_path("http://0.0.0.0" + std::string{request.target()}), raw_request_(request) {
        parsed_url = boost::urls::parse_uri(full_path);
        if (!parsed_url) {
            throw std::runtime_error("Failed to parse URL");
        }
    }

    [[nodiscard]] auto version() const -> unsigned {
        return raw_request_.version();
    }

    [[nodiscard]] auto keep_alive() const -> bool {
        return raw_request_.keep_alive();
    }

    [[nodiscard]] auto path() const -> std::string {
        return parsed_url->path();
    }

    [[nodiscard]] auto headers() const -> std::unordered_map<std::string, std::string> {
        std::unordered_map<std::string, std::string> headers;
        for (const auto& header : raw_request_.base()) {
            headers[header.name_string()] = header.value();
        }
        return headers;
    }

    [[nodiscard]] auto params() const -> std::unordered_map<std::string, std::string> {
        std::unordered_map<std::string, std::string> params;
        for (const auto& param : parsed_url->params()) {
            params[param.key] = param.value;
        }
        return params;
    }

    [[nodiscard]] auto body() const -> std::string {
        return raw_request_.body();
    }

    [[nodiscard]] auto raw_request() const -> const http_request& {
        return raw_request_;
    }

   private:
    const std::string full_path;
    const http_request& raw_request_;
    boost::system::result<boost::url_view> parsed_url;
};

auto health(const parsed_request_t& request) -> http::message_generator {
    http::response<http::empty_body> response{http::status::ok, request.version()};
    response.set(http::field::content_type, MIME_TYPE_APPLICATION_JSON);
    response.keep_alive(request.keep_alive());
    response.prepare_payload();
    return response;
}

auto simulate(const parsed_request_t& request) -> http::message_generator {
    if (auto headers = request.headers(); !headers.contains("Content-Type") ||
                                          headers["Content-Type"] != MIME_TYPE_APPLICATION_JSON) {
        return bad_request(request.raw_request(), "Content-Type must be application/json");
    }

    const auto& request_body = request.body();
    if (request_body.empty()) {
        return bad_request(request.raw_request(), "Request body must not be empty");
    }

    std::string response_body;
    try {
        const auto encounter = nlohmann::json::parse(request_body).get<configuration::encounter_t>();
        response_body = combat_loop(encounter, encounter.enable_caching);
    } catch (const std::exception& err) {
        spdlog::error("error: {}", err.what());
        return bad_request(request.raw_request(), err.what());
    }

    http_response response{http::status::ok, request.version()};
    response.set(http::field::content_type, MIME_TYPE_APPLICATION_JSON);
    response.keep_alive(request.keep_alive());
    response.body() = std::move(response_body);
    response.prepare_payload();
    return response;
}

auto handle_request(const http_request&& request) -> http::message_generator {
    if (request.method() != http::verb::get) {
        return bad_request(request, "Only GET method is supported");
    }

    // spdlog::debug("Received request for {}", std::string{request.target()});

    // if (request.method() == http::verb::head) {
    //     http::response<http::empty_body> response{http::status::ok, request.version()};
    //     response.set(http::field::content_type, MIME_TYPE_APPLICATION_JSON);
    //     response.content_length(0);
    //     response.keep_alive(request.keep_alive());
    //     return response;
    // }

    const parsed_request_t parsed_request{request};

    const std::string& path = parsed_request.path();
    spdlog::info("Received request for {}", path);

    // spdlog::debug("Path: {}", path);
    // spdlog::debug("Content-Type: {}", headers.find("Content-Type")->value());
    // if (!request_body.empty()) {
    //     spdlog::debug("Body: {}", request_body);
    // }

    if (path == "/health") {
        return health(parsed_request);
    }
    if (path == "/simulate") {
        return simulate(parsed_request);
    }

    http::response<http::empty_body> response{http::status::not_found, request.version()};
    response.set(http::field::content_type, MIME_TYPE_TEXT_PLAIN);
    response.keep_alive(request.keep_alive());
    response.prepare_payload();
    return response;
}

class session_t : public std::enable_shared_from_this<session_t> {
   public:
    explicit session_t(tcp::socket&& socket) : stream_{std::move(socket)} {
    }

    void begin() {
        boost::asio::dispatch(
            stream_.get_executor(),
            boost::beast::bind_front_handler(&session_t::read_request, shared_from_this()));
    }

    void read_request() {
        req_ = {};
        stream_.expires_after(std::chrono::seconds(30));
        http::async_read(stream_,
                         buffer_,
                         req_,
                         boost::beast::bind_front_handler(&session_t::on_read, shared_from_this()));
    }

    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec == http::error::end_of_stream) {
            return do_close();
        }
        if (ec) {
            spdlog::error("on_read: {}", ec.message());
            return;
        }
        send_response(handle_request(std::move(req_)));
    }

    void send_response(http::message_generator&& msg) {
        bool keep_alive = msg.keep_alive();
        boost::beast::async_write(
            stream_,
            std::move(msg),
            boost::beast::bind_front_handler(&session_t::on_write, shared_from_this(), keep_alive));
    }

    void on_write(const bool keep_alive,
                  boost::beast::error_code ec,
                  std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec) {
            spdlog::error("on_write: {}", ec.message());
            return;
        }
        if (!keep_alive) {
            return do_close();
        }
        read_request();
    }

    void do_close() {
        boost::beast::error_code ec;
        ec = stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
        if (ec) {
            spdlog::error("do_close: {}", ec.message());
            std::exit(1);
        }
    }

   private:
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
};

class connection_listener_t : public std::enable_shared_from_this<connection_listener_t> {
   public:
    connection_listener_t(boost::asio::io_context& io_context,
                          const boost::asio::ip::tcp::endpoint& endpoint)
        : io_context_(io_context), acceptor_{boost::asio::make_strand(io_context)} {
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen(boost::asio::socket_base::max_listen_connections);
    }

    auto begin() -> void {
        spdlog::info("Listening on {}:{}",
                     acceptor_.local_endpoint().address().to_string(),
                     acceptor_.local_endpoint().port());
        accept_connections();
    }

   private:
    auto accept_connections() -> void {
        acceptor_.async_accept(boost::asio::make_strand(io_context_),
                               boost::beast::bind_front_handler(
                                   &connection_listener_t::on_connection, shared_from_this()));
    }

    auto on_connection(boost::beast::error_code ec, tcp::socket socket) -> void {
        if (ec) {
            spdlog::error("on_connection: {}", ec.message());
            return;
        }
        std::make_shared<session_t>(std::move(socket))->begin();
        accept_connections();
    }

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
};

auto start_server_http(const http_server_config_t& config) -> void {
    boost::asio::io_context io_context{config.threads};

    boost::asio::signal_set signals{io_context, SIGINT, SIGTERM};
    signals.async_wait([&](auto, auto) { io_context.stop(); });

    const auto endpoint = boost::asio::ip::tcp::endpoint{
        boost::asio::ip::make_address(config.server_host), config.server_port};
    std::make_shared<connection_listener_t>(io_context, endpoint)->begin();

    std::vector<std::thread> threads;
    threads.reserve(config.threads - 1);
    for (auto i = config.threads - 1; i > 0; --i) {
        threads.emplace_back([&io_context] { io_context.run(); });
    }
    io_context.run();
}

}  // namespace gw2combat

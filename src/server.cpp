#include "server.hpp"

#include "asio/asio.hpp"

#include "combat_loop.hpp"

namespace gw2combat {

using tcp = asio::ip::tcp;

asio::awaitable<void> request_handler(tcp::socket socket) {
    try {
        while (socket.is_open()) {
            asio::streambuf buffer;
            std::size_t bytes_read =
                co_await asio::async_read_until(socket, buffer, "\n", asio::use_awaitable);
            if (bytes_read == 0) {
                throw std::runtime_error("read nothing from socket!");
            }
            std::istream istream{&buffer};
            std::string payload;
            std::getline(istream, payload);

            auto encounter = nlohmann::json::parse(payload).get<configuration::encounter_t>();
            auto simulation_result_json = combat_loop(encounter);
            co_await asio::async_write(
                socket,
                asio::buffer(simulation_result_json, simulation_result_json.size()),
                asio::use_awaitable);
        }
    } catch (asio::system_error& e) {
    } catch (std::exception& e) {
        spdlog::error("Exception: {}", e.what());
    }
}

asio::awaitable<void> connection_listener(const std::string& hostname, int port) {
    auto executor = co_await asio::this_coro::executor;
    auto endpoint = asio::ip::tcp::endpoint{asio::ip::address_v4::from_string(hostname),
                                            static_cast<asio::ip::port_type>(port)};
    spdlog::info("Starting server at {}:{}", endpoint.address().to_string(), endpoint.port());
    tcp::acceptor server_socket{executor, endpoint};
    while (true) {
        tcp::socket socket = co_await server_socket.async_accept(asio::use_awaitable);
        asio::co_spawn(executor, request_handler(std::move(socket)), asio::detached);
    }
}

void start_server(const std::string& hostname, int port) {
    asio::io_context io_context;
    asio::signal_set signals{io_context, SIGINT, SIGTERM};
    signals.async_wait([&](auto, auto) { io_context.stop(); });

    asio::co_spawn(io_context, connection_listener(hostname, port), asio::detached);

    io_context.run();
}

}  // namespace gw2combat

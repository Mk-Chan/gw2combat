#include "server.hpp"

#include "asio/asio.hpp"

#include "combat_loop.hpp"

namespace gw2combat {

using tcp = asio::ip::tcp;

asio::awaitable<void> request_handler(tcp::socket socket) {
    try {
        while (socket.is_open()) {
            std::string buffer;
            for (std::string temp_buffer;;) {
                try {
                    std::size_t bytes_read = co_await asio::async_read_until(
                        socket, asio::dynamic_buffer(temp_buffer, 1024), "\n", asio::use_awaitable);
                    if (bytes_read == 0) {
                        break;
                    }
                    buffer.append(temp_buffer.substr(0, bytes_read));
                    temp_buffer.erase(0, bytes_read);
                } catch (std::exception& e) {
                    break;
                }
            }
            std::string encounter_configuration{buffer};
            auto simulation_result_json = server_combat_loop(encounter_configuration);
            co_await asio::async_write(
                socket,
                asio::buffer(simulation_result_json, simulation_result_json.size()),
                asio::use_awaitable);
        }
    } catch (std::exception& e) {
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

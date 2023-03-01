#include "argparse/argparse.hpp"
#include "combat_loop.hpp"
#include "server.hpp"

int main(int argc, char** argv) {
    argparse::ArgumentParser parser{"gw2combat"};
    parser.add_argument("--search-rotation")
        .default_value(std::string{""})
        .help("Server mode with hostname:port configuration.");
    parser.add_argument("--server")
        .default_value(std::string{"127.0.0.1:54321"})
        .help("Server mode with hostname:port configuration.");
    parser.add_argument("--encounter")
        .default_value(std::string{"resources/encounter.json"})
        .help("Path to encounter file. Only applicable in default mode.");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    bool server_mode = parser.is_used("--server");
    bool search_rotation_mode = parser.is_used("--search-rotation");
    if (!server_mode && !search_rotation_mode) {
        const auto& encounter = parser.get<std::string>("--encounter");
        gw2combat::local_combat_loop(encounter);
    } else if (search_rotation_mode) {
        const auto& encounter = parser.get<std::string>("--encounter");
        gw2combat::search_rotation_for_encounter(encounter);
    } else {
        const auto& server_configuration = parser.get<std::string>("--server");
        auto delimiter_index = server_configuration.find(':');
        const std::string& hostname = server_configuration.substr(0, delimiter_index);
        int port = std::stoi(
            server_configuration.substr(delimiter_index + 1, server_configuration.size()));
        gw2combat::start_server(hostname, port);
    }
    return 0;
}

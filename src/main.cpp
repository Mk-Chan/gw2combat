#include <cfenv>
#include <fstream>

#include "combat_loop.hpp"
#include "server.hpp"

#include "configuration/build.hpp"
#include "configuration/encounter-local.hpp"
#include "configuration/encounter.hpp"

#include "spdlog/spdlog.h"
#include "utils/io_utils.hpp"

#include "argparse/argparse.hpp"

using namespace gw2combat;

configuration::encounter_t convert_encounter(
    const configuration::encounter_local_t& encounter_local);

int main(int argc, char** argv) {
    if (int error = std::fesetround(FE_TONEAREST); error) {
        spdlog::warn(
            "Unable to use banker's/half-to-even rounding! Expect minor inaccuracies in "
            "simulation.");
    }

    argparse::ArgumentParser parser{"gw2combat"};
    parser.add_argument("--server")
        .default_value(std::string{"127.0.0.1:54321"})
        .help("Server mode with hostname:port configuration.");
    parser.add_argument("--encounter")
        .default_value(std::string{"resources/encounter.json"})
        .help("Path to encounter file. Only applicable in default mode.");
    parser.add_argument("--audit-path")
        .default_value(std::string{"audit.json"})
        .help("Path to audit file. Only applicable in default mode.");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    bool server_mode = parser.is_used("--server");
    if (!server_mode) {
        const auto& encounter_path = parser.get<std::string>("--encounter");
        const auto& audit_path = parser.get<std::string>("--audit-path");
        auto encounter_local = utils::read<configuration::encounter_local_t>(encounter_path);
        auto encounter = convert_encounter(encounter_local);
        std::ofstream audit_path_stream{audit_path, std::ios::trunc};

        auto simulation_result_json = combat_loop(encounter);

        audit_path_stream << simulation_result_json;
    } else {
        const auto& server_configuration = parser.get<std::string>("--server");
        auto delimiter_index = server_configuration.find(':');
        const std::string& hostname = server_configuration.substr(0, delimiter_index);
        int port = std::stoi(
            server_configuration.substr(delimiter_index + 1, server_configuration.size()));
        start_server_tcp(hostname, port);
    }
    return 0;
}

configuration::encounter_t convert_encounter(
    const configuration::encounter_local_t& encounter_local) {
    configuration::encounter_t converted_encounter;
    for (auto&& actor : encounter_local.actors) {
        auto build = utils::read<configuration::build_t>(actor.build_path);
        configuration::rotation_t converted_rotation;
        if (!actor.rotation_path.empty()) {
            if (actor.rotation_path.ends_with(".json")) {
                auto rotation = utils::read<configuration::rotation_t>(actor.rotation_path);
                for (auto&& skill_cast : rotation.skill_casts) {
                    converted_rotation.skill_casts.emplace_back(
                        configuration::skill_cast_t{skill_cast.skill, skill_cast.cast_time_ms});
                }
            } else if (actor.rotation_path.ends_with(".csv")) {
                std::basic_ifstream<char> ifstream{actor.rotation_path, std::ios::in};

                int cast_time_offset_ms = 0;
                int line_num = -1;
                for (std::string line; std::getline(ifstream, line); ++line_num) {
                    if (line_num == -1) {
                        continue;  // Skip first line (expected to be "rotation")
                    }

                    auto delimiter_pos = line.find(',');
                    if (delimiter_pos == std::string::npos) {
                        throw std::runtime_error(
                            fmt::format("unable to read rotation. line_num: {}", line_num));
                    }

                    std::string skill_name = line.substr(0, delimiter_pos);
                    std::string time_str = line.substr(delimiter_pos + 2);

                    auto last_delimiter_pos = time_str.find(',');
                    if (last_delimiter_pos == std::string::npos) {
                        last_delimiter_pos = time_str.length();
                    }
                    last_delimiter_pos -= 6;

                    int cast_time_ms = utils::round_down(
                        std::stod(time_str.substr(6, last_delimiter_pos - 1)) * 1'000.0);
                    if (line_num == 0) {
                        cast_time_offset_ms = -cast_time_ms;
                    }
                    converted_rotation.skill_casts.emplace_back(configuration::skill_cast_t{
                        skill_name, cast_time_ms + cast_time_offset_ms});
                }
            }
        }
        converted_encounter.actors.emplace_back(configuration::actor_t{
            .name = actor.name,
            .build = build,
            .rotation = converted_rotation,
            .team = actor.team,
            .audit_base_path = actor.audit_base_path,
        });
    }
    std::copy(encounter_local.termination_conditions.begin(),
              encounter_local.termination_conditions.end(),
              std::back_inserter(converted_encounter.termination_conditions));
    converted_encounter.audit_configuration = encounter_local.audit_configuration;
    converted_encounter.require_afk_skills = encounter_local.require_afk_skills;
    converted_encounter.audit_offset = encounter_local.audit_offset;
    converted_encounter.weapon_strength_mode = encounter_local.weapon_strength_mode;
    converted_encounter.enable_caching = false;
    return converted_encounter;
}

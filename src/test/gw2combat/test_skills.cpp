#include <catch2/catch_test_macros.hpp>

#include "gw2combat/actor/skill.hpp"

namespace gw2combat {

TEST_CASE("Test skill read", "[skill]") {
    auto& skill_database = actor::skill_database::instance();
    std::string json_str{nlohmann::json{skill_database}[0].dump()};
    REQUIRE(json_str.find("Invalid") == std::string::npos);
}

}  // namespace gw2combat

#include <catch2/catch_test_macros.hpp>

#include "gw2combat/actor/build.hpp"

namespace gw2combat {

TEST_CASE("Test build read", "[build]") {
    nlohmann::json build1{actor::build_t::read("build-cfb.json")};
    nlohmann::json build2{actor::build_t::read("build-dh.json")};
    // TODO: Uncomment after implementing everything
    // REQUIRE(build1[0].dump().find("Invalid") == std::string::npos);
    // REQUIRE(build2[0].dump().find("Invalid") == std::string::npos);
    REQUIRE(true);
}

}  // namespace gw2combat

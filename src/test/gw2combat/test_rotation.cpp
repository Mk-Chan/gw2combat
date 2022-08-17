#include <catch2/catch_test_macros.hpp>

#include "gw2combat/actor/rotation.hpp"

namespace gw2combat {

TEST_CASE("Test rotation read", "[rotation]") {
    auto rotation1 = actor::rotation_t::read("rotation-cfb.csv", actor::base_class_t::GUARDIAN);
    auto rotation2 = actor::rotation_t::read("rotation-dh.csv", actor::base_class_t::GUARDIAN);
    REQUIRE(true);
}

}  // namespace gw2combat

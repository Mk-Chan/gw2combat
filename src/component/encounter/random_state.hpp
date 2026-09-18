#ifndef GW2COMBAT_COMPONENT_ENCOUNTER_RANDOM_STATE_HPP
#define GW2COMBAT_COMPONENT_ENCOUNTER_RANDOM_STATE_HPP

#include <random>

#include "common.hpp"

namespace gw2combat::component {

struct random_state_t {
    std::mt19937 generator;

    explicit random_state_t(std::int64_t seed) {
        if (seed < 0) {
            generator.seed(std::random_device{}());
        } else {
            std::seed_seq words{static_cast<std::uint32_t>(seed),
                                static_cast<std::uint32_t>(static_cast<std::uint64_t>(seed) >> 32)};
            generator.seed(words);
        }
    }
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ENCOUNTER_RANDOM_STATE_HPP

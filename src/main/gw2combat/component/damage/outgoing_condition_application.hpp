#ifndef GW2COMBAT_OUTGOING_CONDITION_APPLICATION_HPP
#define GW2COMBAT_OUTGOING_CONDITION_APPLICATION_HPP

#include "gw2combat/effect.hpp"

namespace gw2combat::component {

struct outgoing_condition_application {
    inline void append_burning_effects(const effect effect, size_t num_stacks) {
        for (size_t i = 0; i < num_stacks; ++i) {
            burning_effects.push_back(effect);
        }
    }

    std::vector<effect> burning_effects;
    std::optional<effect> binding_blade_effect = std::nullopt;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_OUTGOING_CONDITION_APPLICATION_HPP

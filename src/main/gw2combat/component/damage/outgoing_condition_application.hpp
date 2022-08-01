#ifndef GW2COMBAT_COMPONENT_DAMAGE_OUTGOING_CONDITION_APPLICATION_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_OUTGOING_CONDITION_APPLICATION_HPP

#include "gw2combat/effects.hpp"

namespace gw2combat::component {

struct outgoing_condition_application {
    inline void append_burning_effects(const effects::burning effect, size_t num_stacks) {
        for (size_t i = 0; i < num_stacks; ++i) {
            burning_effects.push_back(effect);
        }
    }
    inline void append_bleeding_effects(const effects::bleeding effect, size_t num_stacks) {
        for (size_t i = 0; i < num_stacks; ++i) {
            bleeding_effects.push_back(effect);
        }
    }

    std::vector<effects::burning> burning_effects;
    std::vector<effects::bleeding> bleeding_effects;
    std::optional<effects::binding_blade> binding_blade_effect = std::nullopt;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_OUTGOING_CONDITION_APPLICATION_HPP

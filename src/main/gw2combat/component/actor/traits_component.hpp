#ifndef GW2COMBAT_COMPONENT_ACTOR_TRAITS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ACTOR_TRAITS_COMPONENT_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/utilities/base_utilities.hpp"

#include "gw2combat/actor/trait.hpp"

namespace gw2combat::component {

struct trait_entity {
    actor::trait_t trait;
    entity_t entity;
};

struct traits_component {
    [[nodiscard]] inline entity_t find_by(const actor::trait_t& trait) const {
        for (auto& trait_entity : trait_entities) {
            if (trait_entity.trait == trait) {
                return trait_entity.entity;
            }
        }
        throw std::runtime_error(
            fmt::format("trait with key {} not found!", utils::to_string(trait)));
    }

    std::vector<trait_entity> trait_entities;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_TRAITS_COMPONENT_HPP

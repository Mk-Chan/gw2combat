#ifndef GW2COMBAT_COMPONENT_ACTOR_UNIQUE_EFFECTS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ACTOR_UNIQUE_EFFECTS_COMPONENT_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/utilities/base_utilities.hpp"

#include "gw2combat/actor/unique_effect.hpp"

namespace gw2combat::component {

struct unique_effect_entity {
    actor::unique_effect_t unique_effect;
    entity_t entity;
};

struct unique_effects_component {
    [[nodiscard]] inline bool has(const actor::unique_effect_t& unique_effect) const {
        return std::any_of(unique_effect_entities.cbegin(),
                           unique_effect_entities.cend(),
                           [&](const unique_effect_entity& unique_effect_entity) {
                               return unique_effect == unique_effect_entity.unique_effect;
                           });
    }
    [[nodiscard]] inline size_t count(const actor::unique_effect_t& unique_effect) const {
        size_t count = 0;
        for (auto& unique_effect_entity : unique_effect_entities) {
            if (unique_effect_entity.unique_effect == unique_effect) {
                ++count;
            }
        }
        return count;
    }
    [[nodiscard]] inline std::vector<entity_t> find_by(
        const actor::unique_effect_t& unique_effect) const {
        std::vector<entity_t> entities;
        for (auto& unique_effect_entity : unique_effect_entities) {
            if (unique_effect_entity.unique_effect == unique_effect) {
                entities.emplace_back(unique_effect_entity.entity);
            }
        }
        return entities;
    }

    std::vector<unique_effect_entity> unique_effect_entities;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(unique_effect_entity, unique_effect, entity)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(unique_effects_component, unique_effect_entities)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_UNIQUE_EFFECTS_COMPONENT_HPP

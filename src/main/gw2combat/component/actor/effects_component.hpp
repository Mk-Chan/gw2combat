#ifndef GW2COMBAT_COMPONENT_EFFECTS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_EFFECTS_COMPONENT_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/effect/effect.hpp"

namespace gw2combat::component {

struct effect_entity {
    effect::effect_t effect;
    entity_t entity;
};

struct effects_component {
    [[nodiscard]] inline std::vector<entity_t> find_by(const effect::effect_t& effect) const {
        std::vector<entity_t> entities;
        for (auto& effect_entity : effect_entities) {
            if (effect_entity.effect == effect) {
                entities.emplace_back(effect_entity.entity);
            }
        }
        return entities;
    }

    std::vector<effect_entity> effect_entities;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECTS_COMPONENT_HPP

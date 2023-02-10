#ifndef GW2COMBAT_COMPONENT_ACTOR_EFFECTS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ACTOR_EFFECTS_COMPONENT_HPP

#include "common.hpp"

#include "actor/effect.hpp"

#include "utils/effect_utils.hpp"

namespace gw2combat::component {

struct effect_entity {
    actor::effect_t effect;
    entity_t entity;
};

struct effects_component {
    [[nodiscard]] inline int count(const actor::effect_t& effect) const {
        return std::min(utils::get_max_considered_stacks_of_effect_type(effect),
                        (int)std::count_if(effect_entities.begin(),
                                           effect_entities.end(),
                                           [&](const effect_entity& effect_entity) {
                                               return effect_entity.effect == effect;
                                           }));
    }
    [[nodiscard]] inline std::unordered_map<actor::effect_t, int> count_all() const {
        std::unordered_map<actor::effect_t, int> effect_counts;
        for (auto effect_entity : effect_entities) {
            effect_counts[effect_entity.effect] =
                std::min(utils::get_max_considered_stacks_of_effect_type(effect_entity.effect),
                         effect_counts[effect_entity.effect] + 1);
        }
        return effect_counts;
    }
    [[nodiscard]] inline bool has(const actor::effect_t& effect) const {
        return std::any_of(
            effect_entities.begin(),
            effect_entities.end(),
            [&](const effect_entity& effect_entity) { return effect_entity.effect == effect; });
    }
    [[nodiscard]] inline std::vector<entity_t> find_by(const actor::effect_t& effect) const {
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(effect_entity, effect, entity)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(effects_component, effect_entities)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_EFFECTS_COMPONENT_HPP

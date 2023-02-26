#ifndef GW2COMBAT_COMPONENT_ACTOR_COUNTERS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ACTOR_COUNTERS_COMPONENT_HPP

#include "actor/counter.hpp"

namespace gw2combat::component {

struct counter_entity {
    actor::counter_t counter;
    entity_t entity = entt::tombstone;
};

struct counters_component {
    [[nodiscard]] inline bool has(const actor::counter_t& counter) const {
        return std::any_of(counter_entities.cbegin(),
                           counter_entities.cend(),
                           [&](const counter_entity& counter_entity) {
                               return counter == counter_entity.counter;
                           });
    }
    [[nodiscard]] inline std::optional<entity_t> find_by(const actor::counter_t& counter) const {
        auto counter_entity_iter = std::find_if(counter_entities.cbegin(),
                                                counter_entities.cend(),
                                                [&](const counter_entity& counter_entity) {
                                                    return counter == counter_entity.counter;
                                                });
        if (counter_entity_iter == counter_entities.cend()) {
            return std::nullopt;
        } else {
            return counter_entity_iter->entity;
        }
    }

    std::vector<counter_entity> counter_entities;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(counter_entity, counter, entity)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(counters_component, counter_entities)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_COUNTERS_COMPONENT_HPP

#ifndef GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP

#include "common.hpp"

#include "actor/skill.hpp"

namespace gw2combat::component {

struct strike_t {
    entity_t skill_entity = entt::tombstone;
    int num_targets = 1;
    double weapon_strength_roll = 0.0;

    constexpr inline bool operator==(const strike_t& rhs) const {
        return this->skill_entity == rhs.skill_entity;
    }
};

struct incoming_strike {
    entity_t source_entity = entt::tombstone;
    strike_t strike;
};

struct outgoing_strikes_component {
    std::vector<strike_t> strikes;
};

struct incoming_strikes_component {
    std::vector<incoming_strike> strikes;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(strike_t,
                                                skill_entity,
                                                num_targets,
                                                weapon_strength_roll)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_strike, source_entity, strike)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(outgoing_strikes_component, strikes)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_strikes_component, strikes)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP

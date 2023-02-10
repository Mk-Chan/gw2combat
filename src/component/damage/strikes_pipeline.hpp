#ifndef GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP

#include "common.hpp"

#include "actor/skill.hpp"

namespace gw2combat::component {

struct strike_t {
    actor::skill_t skill;
    double outgoing_damage;
    double critical_chance_pct;
    double critical_damage_pct;
    int num_targets = 1;

    constexpr inline bool operator==(const strike_t& rhs) const {
        return this->skill == rhs.skill;
    }
};

struct incoming_strike {
    entity_t source_entity;
    strike_t strike;
};

struct outgoing_strikes_component {
    std::vector<strike_t> strikes;
};

struct incoming_strikes_component {
    std::vector<incoming_strike> strikes;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(strike_t,
                                                skill,
                                                outgoing_damage,
                                                critical_chance_pct,
                                                critical_damage_pct,
                                                num_targets)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_strike, source_entity, strike)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(outgoing_strikes_component, strikes)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_strikes_component, strikes)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP

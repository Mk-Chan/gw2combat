#ifndef GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP

#include "common.hpp"

#include "actor/skill.hpp"

#include "configuration/effect_application.hpp"

namespace gw2combat::component {

struct strike_t {
    entity_t skill_entity = entt::tombstone;
    int num_targets = 1;
    double flat_damage = 0.0;
    double weapon_strength = 0.0;
    double damage_coefficient = 0.0;
    bool can_critical_strike = true;
    std::vector<configuration::effect_application_t> on_strike_effect_applications{};
    std::vector<actor::skill_tag_t> tags;

    constexpr inline bool operator==(const strike_t& rhs) const {
        return this->skill_entity == rhs.skill_entity;
    }
};
struct outgoing_strikes_component {
    std::vector<strike_t> strikes;
};

struct incoming_strike {
    entity_t source_entity = entt::tombstone;
    strike_t strike;
    bool is_critical = false;
};
struct incoming_strikes_component {
    std::vector<incoming_strike> strikes;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(strike_t,
                                                skill_entity,
                                                num_targets,
                                                flat_damage,
                                                weapon_strength,
                                                damage_coefficient,
                                                can_critical_strike,
                                                on_strike_effect_applications,
                                                tags)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(outgoing_strikes_component, strikes)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_strike,
                                                source_entity,
                                                strike,
                                                is_critical)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_strikes_component, strikes)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP

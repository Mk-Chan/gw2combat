#ifndef GW2COMBAT_STRIKE_HPP
#define GW2COMBAT_STRIKE_HPP

#include "gw2combat/actor/skill.hpp"
#include "gw2combat/effect/application.hpp"

namespace gw2combat::strike {

struct strike_t {
    entity_t entity;
    actor::skill_t skill;
    double base_outgoing_damage;
    double critical_chance_pct;
    double outgoing_strike_damage_multiplier;
    std::vector<effect::application_t> on_hit_effect_applications;
    std::vector<effect::application_t> on_pulse_effect_applications;

    constexpr inline bool operator==(const strike_t& rhs) const {
        return this->entity == rhs.entity && this->skill == rhs.skill;
    }
};

}  // namespace gw2combat::strike

#endif  // GW2COMBAT_STRIKE_HPP

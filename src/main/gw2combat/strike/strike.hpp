#ifndef GW2COMBAT_STRIKE_HPP
#define GW2COMBAT_STRIKE_HPP

#include "gw2combat/actor/skill.hpp"
#include "gw2combat/effect/application.hpp"

namespace gw2combat::strike {

struct strike_t {
    actor::skill_t skill;
    double base_outgoing_damage;
    double critical_chance_pct;
    double critical_damage_pct;
    std::vector<effect::application_t> on_strike_effect_applications;
    int num_targets = 1;

    constexpr inline bool operator==(const strike_t& rhs) const {
        return this->skill == rhs.skill;
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(strike_t,
                                                skill,
                                                base_outgoing_damage,
                                                critical_chance_pct,
                                                critical_damage_pct,
                                                on_strike_effect_applications,
                                                num_targets)

}  // namespace gw2combat::strike

#endif  // GW2COMBAT_STRIKE_HPP

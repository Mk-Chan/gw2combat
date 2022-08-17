#ifndef GW2COMBAT_COMPONENT_SKILL_RECHARGE_HPP
#define GW2COMBAT_COMPONENT_SKILL_RECHARGE_HPP

namespace gw2combat::component {

struct recharge {
    int duration;
    int progress = 0;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILL_RECHARGE_HPP

#ifndef GW2COMBAT_COMPONENT_SKILL_RECHARGE_HPP
#define GW2COMBAT_COMPONENT_SKILL_RECHARGE_HPP

namespace gw2combat::component {

struct recharge {
    int duration;
    int progress = 0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(recharge, duration, progress)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILL_RECHARGE_HPP

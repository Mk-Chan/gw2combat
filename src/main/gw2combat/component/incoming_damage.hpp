#ifndef GW2COMBAT_COMPONENT_INCOMING_DAMAGE_HPP
#define GW2COMBAT_COMPONENT_INCOMING_DAMAGE_HPP

namespace gw2combat::component {

struct incoming_damage {
    struct damage_data {
        entt::entity source;
        double value;
    };
    std::vector<damage_data> damage_data_vec;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_INCOMING_DAMAGE_HPP

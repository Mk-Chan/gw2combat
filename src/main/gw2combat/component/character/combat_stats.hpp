#ifndef GW2COMBAT_COMPONENT_COMBAT_STATS_HPP
#define GW2COMBAT_COMPONENT_COMBAT_STATS_HPP

namespace gw2combat::component {

struct combat_stats {
    combat_stats(unsigned int health, unsigned int endurance)
        : health(health), endurance(endurance) {
    }
    unsigned int health;
    unsigned int endurance;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_COMBAT_STATS_HPP

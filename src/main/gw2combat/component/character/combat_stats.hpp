#ifndef GW2COMBAT_COMPONENT_COMBAT_STATS_HPP
#define GW2COMBAT_COMPONENT_COMBAT_STATS_HPP

namespace gw2combat::component {

struct combat_stats {
    combat_stats(unsigned int health, unsigned int endurance)
        : health(health), endurance(endurance) {
    }
    unsigned int health;
    unsigned int endurance;

    struct damage_stats {
        double total_damage;
        double average_damage;
        unsigned int num_hits;

        void update(double damage) {
            total_damage += damage;
            num_hits += 1;
            average_damage = total_damage / (double)num_hits;
        }
    };
    ;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_COMBAT_STATS_HPP

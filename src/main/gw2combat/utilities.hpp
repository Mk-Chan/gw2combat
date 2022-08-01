#ifndef GW2COMBAT_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_HPP

#include <algorithm>

#include "types.hpp"

#include "gw2combat/component/damage/outgoing_damage_source.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline std::string get_name(entity_t entity, registry_t& registry) {
    if (!registry.ctx().contains<entt::hashed_string>(to_u32(entity))) {
        return "temporary_entity";
    }
    return std::string{registry.ctx().at<entt::hashed_string>(to_u32(entity)).data()};
}

[[nodiscard]] static inline entity_t get_damage_source_entity(entity_t entity,
                                                              registry_t& registry) {
    entity_t current_entity = entity;
    while (true) {
        auto outgoing_damage_source_ptr =
            registry.try_get<component::outgoing_damage_source>(current_entity);
        if (outgoing_damage_source_ptr == nullptr) {
            break;
        }

        current_entity = outgoing_damage_source_ptr->source;
    }
    return current_entity;
}

static inline int critical_chance_pct_to_precision(double critical_chance_pct) {
    return (int)(critical_chance_pct * 21.0 + 895.0);
}

static inline double calculate_critical_chance_multiplier(unsigned int precision,
                                                          unsigned int ferocity) {
    double critical_chance = 0.05 + ((double)precision - 1000.0) / 2100.0;
    double critical_damage = 1.5 + (double)ferocity / 1500.0;
    return 1.0 + std::min(critical_chance, 1.0) * (critical_damage - 1.0);
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_HPP

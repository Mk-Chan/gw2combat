#ifndef GW2COMBAT_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_HPP

#include <algorithm>

#include "types.hpp"

#include "effects.hpp"
#include "gw2combat/component/damage/source_entity.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline std::string get_name(entity_t entity, registry_t& registry) {
    if (!registry.ctx().contains<entt::hashed_string>(to_u32(entity))) {
        return "temporary_entity";
    }
    return std::string{registry.ctx().at<entt::hashed_string>(to_u32(entity)).data()};
}

[[nodiscard]] static inline entity_t get_source_entity(entity_t entity, registry_t& registry) {
    entity_t current_entity = entity;
    while (true) {
        auto outgoing_damage_source_ptr =
            registry.try_get<component::source_entity>(current_entity);
        if (outgoing_damage_source_ptr == nullptr) {
            break;
        }

        current_entity = outgoing_damage_source_ptr->entity;
    }
    return current_entity;
}

[[nodiscard]] static inline double get_critical_hit_multiplier(
    const component::effective_attributes& effective_attributes) {
    return (1.0 + (std::min(effective_attributes.critical_chance_pct, 100.0) / 100.0) *
                      (effective_attributes.critical_damage_pct / 100.0 - 1.0));
}

[[nodiscard]] static inline tick_t get_effective_condition_duration(
    tick_t duration,
    effects::applied_effect_type effect_type,
    const component::effective_attributes& effective_attributes) {
    auto calc = [&](double special_condition_duration_pct) {
        return tick_t{(unsigned int)((double)duration *
                                     (1.0 + std::max(effective_attributes.condition_duration_pct,
                                                     special_condition_duration_pct) /
                                                100.0))};
    };

    switch (effect_type) {
        case effects::applied_effect_type::BURNING:
            return calc(effective_attributes.burning_duration_pct);
        case effects::applied_effect_type::BLEEDING:
            return calc(effective_attributes.bleeding_duration_pct);
        case effects::applied_effect_type::BINDING_BLADE:
            return tick_t{duration};
    }
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

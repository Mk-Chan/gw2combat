#include "gw2combat/system/system.hpp"

#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/damage/metrics/damage_metrics.hpp"
#include "gw2combat/component/damage/multipliers/incoming_strike_damage_multiplier.hpp"

namespace gw2combat::system {

struct damage_unit {
    entity_t source;
    std::string damage_name;
    double damage;
};

void incoming_strike_damage_calculation(registry_t& registry, tick_t current_tick) {
    auto calculate_strike_damage = [](const strike& strike,
                                      double incoming_strike_damage_multiplier) {
        double effective_critical_hit_multiplier =
            (utils::skill_has_tag(strike.skill, skills::skill_tag::CANNOT_CRIT)
                 ? 1
                 : strike.critical_hit_multiplier);
        return strike.outgoing_strike_damage_multiplier * effective_critical_hit_multiplier *
               utils::get_weapon_strength(strike.skill.weapon_type) *
               strike.skill.damage_coefficient * incoming_strike_damage_multiplier;
    };
    registry.view<component::incoming_strike_damage_multiplier, component::incoming_strike_damage>()
        .each([&](entity_t entity,
                  const component::incoming_strike_damage_multiplier&
                      incoming_strike_damage_multiplier,
                  const component::incoming_strike_damage& incoming_strike_damage) {
            auto damage_units_view =
                incoming_strike_damage.strikes |
                ranges::views::transform([&](const strike& strike) {
                    return damage_unit{strike.source,
                                       strike.skill.name,
                                       calculate_strike_damage(
                                           strike, incoming_strike_damage_multiplier.multiplier)};
                });

            // NOTE: Metrics
            auto damage_metrics_component_ptr =
                registry.try_get<component::damage_metrics_component>(entity);
            if (damage_metrics_component_ptr) {
                for (const auto& damage_unit : damage_units_view) {
                    std::string source_name = utils::get_entity_name(
                        utils::get_source_entity(damage_unit.source, registry), registry);
                    damage_metrics_component_ptr->metrics.emplace_back(
                        component::damage_metrics_component::metric_unit{current_tick,
                                                                         source_name,
                                                                         damage_unit.damage_name,
                                                                         damage_unit.damage});
                    spdlog::info(
                        "tick: {}, entity: {}, incoming strike damage: {}, from skill: {}, by: {}",
                        current_tick,
                        utils::get_entity_name(entity, registry),
                        damage_unit.damage,
                        damage_unit.damage_name,
                        source_name);
                }
            }

            auto damage_view = damage_units_view |
                               ranges::views::transform([](const struct damage_unit& damage_unit) {
                                   return damage_unit.damage;
                               });
            double accumulated_incoming_damage = ranges::accumulate(damage_view, 0.0);
            auto& effective_incoming_damage =
                registry.get_or_emplace<component::effective_incoming_damage>(entity);
            effective_incoming_damage.value += accumulated_incoming_damage;

            spdlog::info("tick: {}, entity: {}, total incoming strike damage: {}",
                         current_tick,
                         utils::get_entity_name(entity, registry),
                         effective_incoming_damage.value);
        });
}

}  // namespace gw2combat::system

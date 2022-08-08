#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/damage/buffered_condition_damage.hpp"
#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/metrics/damage_metrics.hpp"
#include "gw2combat/component/damage/multipliers/incoming_condition_damage_multiplier.hpp"

namespace gw2combat::system {

void expire_effects(registry_t& registry, tick_t current_tick) {
    registry.view<component::incoming_condition_damage_multiplier, component::effects_component>()
        .each([&](entity_t entity,
                  const component::incoming_condition_damage_multiplier&
                      incoming_condition_damage_multiplier,
                  component::effects_component& effects_component) {
            auto& buffered_condition_damage =
                registry.get_or_emplace<component::buffered_condition_damage>(entity);

            for (auto&& [effect_type, effect_stack] : effects_component.effects) {
                if (effect_stack.progress >= effect_stack.duration) {
                    double effective_condition_damage_from_stack =
                        utils::calculate_condition_damage(effect_type, effect_stack, registry) *
                        utils::is_effect_affected_by_incoming_multiplier(effect_type) *
                        incoming_condition_damage_multiplier.multiplier;
                    buffered_condition_damage.damage_units.emplace_back(
                        component::buffered_condition_damage::damage_unit{
                            effect_type,
                            effect_stack.source,
                            effective_condition_damage_from_stack});
                }
            }
            std::erase_if(effects_component.effects, [&](const auto& item) {
                const auto& [effect_type, effect_stack] = item;
                return effect_stack.progress >= effect_stack.duration;
            });
            if (buffered_condition_damage.damage_units.empty()) {
                registry.remove<component::buffered_condition_damage>(entity);
            }
        });
}

void progress_effects(registry_t& registry, tick_t current_tick) {
    registry.view<component::effects_component>().each(
        [&](component::effects_component& effects_component) {
            for (auto&& [effect_type, effect_stacks] : effects_component.effects) {
                effect_stacks.progress += 1;
            }
        });
}

void incoming_condition_damage_calculation(registry_t& registry, tick_t current_tick) {
    registry.view<component::incoming_condition_damage_multiplier, component::effects_component>()
        .each([&](entity_t entity,
                  const component::incoming_condition_damage_multiplier&
                      incoming_condition_damage_multiplier,
                  component::effects_component& effects_component) {
            auto& buffered_condition_damage =
                registry.get_or_emplace<component::buffered_condition_damage>(entity);

            for (auto&& [effect_type, effect_stack] : effects_component.effects) {
                if (!utils::is_damaging_condition(effect_type)) {
                    continue;
                }
                double effective_condition_damage_from_stack =
                    utils::calculate_condition_damage(effect_type, effect_stack, registry) *
                    (utils::is_effect_affected_by_incoming_multiplier(effect_type)
                         ? incoming_condition_damage_multiplier.multiplier
                         : 1.0);
                buffered_condition_damage.damage_units.emplace_back(
                    component::buffered_condition_damage::damage_unit{
                        effect_type, effect_stack.source, effective_condition_damage_from_stack});

                effect_stack.duration -= effect_stack.progress;
                effect_stack.progress = 0;
            }

            if (buffered_condition_damage.damage_units.empty()) {
                registry.remove<component::buffered_condition_damage>(entity);
            }
        });

    // NOTE: Metrics
    registry.view<component::damage_metrics_component, component::buffered_condition_damage>().each(
        [&](component::damage_metrics_component& damage_metrics_component,
            const component::buffered_condition_damage& buffered_condition_damage) {
            for (const auto& damage_unit : buffered_condition_damage.damage_units) {
                damage_metrics_component.metrics.emplace_back(
                    component::damage_metrics_component::metric_unit{
                        current_tick,
                        utils::get_entity_name(
                            utils::get_source_entity(damage_unit.source, registry), registry),
                        nlohmann::json{damage_unit.effect_type}[0],
                        damage_unit.damage});
            }
        });

    registry.view<component::buffered_condition_damage>().each(
        [&](entity_t entity,
            const component::buffered_condition_damage& buffered_condition_damage) {
            auto damage_view =
                buffered_condition_damage.damage_units |
                ranges::views::transform(
                    [](const component::buffered_condition_damage::damage_unit& damage_unit) {
                        return damage_unit.damage;
                    });
            double effective_buffered_condition_damage = ranges::accumulate(damage_view, 0.0);

            auto& effective_incoming_damage =
                registry.get_or_emplace<component::effective_incoming_damage>(
                    entity, component::effective_incoming_damage{0});
            effective_incoming_damage.value += effective_buffered_condition_damage;

            registry.remove<component::buffered_condition_damage>(entity);
            spdlog::info("tick: {}, entity: {}, incoming condition damage: {}",
                         current_tick,
                         utils::get_entity_name(entity, registry),
                         effective_incoming_damage.value);
        });
}

}  // namespace gw2combat::system

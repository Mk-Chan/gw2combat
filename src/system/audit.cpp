#include "audit.hpp"

#include "utils/entity_utils.hpp"

#include "component/actor/casting_skill.hpp"
#include "component/actor/combat_stats.hpp"
#include "component/actor/finished_casting_skill.hpp"
#include "component/audit/audit_component.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/animation_component.hpp"

namespace gw2combat::system {

void audit_effect_applications(registry_t& registry) {
    registry.view<component::incoming_effects_component, component::audit_component>().each(
        [&](const component::incoming_effects_component& incoming_effects_component,
            component::audit_component& audit_component) {
            for (auto&& [source_entity, effect_application] :
                 incoming_effects_component.effect_applications) {
                auto source_actor =
                    utils::get_entity_name(utils::get_owner(source_entity, registry), registry);
                auto effect = effect_application.effect == actor::effect_t::INVALID
                                  ? ""
                                  : nlohmann::json{effect_application.effect}[0].get<std::string>();
                audit_component.events.emplace_back(audit::effect_application_event_t{
                    .time_ms = utils::get_current_tick(registry),
                    .source_actor = source_actor,
                    .source_skill = effect_application.source_skill,
                    .effect = effect,
                    .unique_effect = effect_application.unique_effect.unique_effect_key,
                    .num_stacks = effect_application.num_stacks,
                    .duration_ms = effect_application.base_duration_ms,
                });
            }
        });
}

void audit_skill_casts(registry_t& registry) {
    registry
        .view<component::casting_skill,
              component::animation_component,
              component::audit_component>()
        .each([&](const component::casting_skill& casting_skill,
                  const component::animation_component& animation_component,
                  component::audit_component& audit_component) {
            if (animation_component.progress[0] + animation_component.progress[1] > 1) {
                return;
            }

            audit_component.events.emplace_back(audit::skill_cast_begin_event_t{
                .time_ms = utils::get_current_tick(registry),
                .skill = registry.get<component::is_skill>(casting_skill.skill_entity)
                             .skill_configuration.skill_key,
            });
        });

    registry.view<component::finished_casting_skill, component::audit_component>().each(
        [&](const component::finished_casting_skill& finished_casting_skill,
            component::audit_component& audit_component) {
            audit_component.events.emplace_back(audit::skill_cast_end_event_t{
                .time_ms = utils::get_current_tick(registry),
                .skill = registry.get<component::is_skill>(finished_casting_skill.skill_entity)
                             .skill_configuration.skill_key,
            });
        });
}

void audit_damage(registry_t& registry) {
    registry.view<component::incoming_damage, component::audit_component>().each(
        [&](const component::incoming_damage& incoming_damage,
            component::audit_component& audit_component) {
            for (auto& incoming_damage_event : incoming_damage.incoming_damage_events) {
                auto damage_type = [&]() {
                    if (incoming_damage_event.effect != actor::effect_t::INVALID) {
                        switch (incoming_damage_event.effect) {
                            case actor::effect_t::BURNING:
                                return audit::damage_event_t::damage_type_t::BURNING;
                            case actor::effect_t::BLEEDING:
                                return audit::damage_event_t::damage_type_t::BLEEDING;
                            case actor::effect_t::TORMENT:
                                return audit::damage_event_t::damage_type_t::TORMENT_STATIONARY;
                            case actor::effect_t::POISON:
                                return audit::damage_event_t::damage_type_t::POISON;
                            case actor::effect_t::CONFUSION:
                                return audit::damage_event_t::damage_type_t::CONFUSION;
                            case actor::effect_t::BINDING_BLADE:
                                return audit::damage_event_t::damage_type_t::BINDING_BLADE;
                            default:
                                throw std::runtime_error("Unknown source for damage!");
                        }
                    } else if (!incoming_damage_event.skill.empty()) {
                        return audit::damage_event_t::damage_type_t::STRIKE;
                    } else {
                        throw std::runtime_error("Unknown source for damage!");
                    }
                }();
                auto source_skill = incoming_damage_event.skill.empty()
                                        ? "unknown_skill"
                                        : incoming_damage_event.skill;
                audit_component.events.emplace_back(audit::damage_event_t{
                    .time_ms = incoming_damage_event.tick,
                    .source_actor =
                        utils::get_entity_name(incoming_damage_event.source_entity, registry),
                    .source_skill = source_skill,
                    .damage_type = damage_type,
                    .damage = static_cast<int>(incoming_damage_event.value),
                });
            }
        });
}

void audit(registry_t& registry) {
    audit_skill_casts(registry);
    audit_effect_applications(registry);
    audit_damage(registry);
}

audit::report_t get_audit_report(registry_t& registry) {
    audit::report_t audit_report;
    registry.view<component::audit_component>().each(
        [&](entity_t actor_entity, const component::audit_component& audit_component) {
            auto& combat_stats = registry.get<component::combat_stats>(actor_entity);
            audit::actor_report_t actor_report{
                .actor = utils::get_entity_name(actor_entity, registry),
                .remaining_health = combat_stats.health,
                .events = audit_component.events,
            };
            audit_report.actors.emplace_back(actor_report);
        });
    return audit_report;
}

}  // namespace gw2combat::system

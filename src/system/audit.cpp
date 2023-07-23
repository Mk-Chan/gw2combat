#include "audit.hpp"

#include "utils/effect_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/skill_utils.hpp"

#include "component/actor/combat_stats.hpp"
#include "component/actor/finished_casting_skills.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/skills_actions_component.hpp"
#include "component/audit/audit_component.hpp"
#include "component/counter/is_counter.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/effect/source_skill.hpp"
#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/cooldown_component.hpp"
#include "component/temporal/duration_component.hpp"
#include "component/temporal/has_alacrity.hpp"

namespace gw2combat::system {

[[nodiscard]] std::vector<audit::skill_cooldown_t> get_skill_cooldowns(entity_t actor_entity,
                                                                       registry_t& registry) {
    std::vector<audit::skill_cooldown_t> skill_cooldowns;
    registry.view<component::cooldown_component, component::ammo, component::is_skill>().each(
        [&](entity_t skill_entity,
            const component::cooldown_component& cooldown_component,
            const component::ammo& ammo,
            const component::is_skill& is_skill) {
            auto owner_entity = registry.get<component::owner_component>(skill_entity).entity;
            if (owner_entity != actor_entity) {
                return;
            }
            bool has_alacrity = registry.any_of<component::has_alacrity>(owner_entity);
            skill_cooldowns.emplace_back(audit::skill_cooldown_t{
                .skill = is_skill.skill_configuration.skill_key,
                .duration = cooldown_component.duration[has_alacrity] -
                            cooldown_component.progress[has_alacrity],
                .remaining_ammo = ammo.current_ammo,
            });
        });
    return skill_cooldowns;
}

[[nodiscard]] std::vector<audit::counter_value_t> get_counter_values(entity_t actor_entity,
                                                                     registry_t& registry) {
    std::vector<audit::counter_value_t> counter_values;
    registry.view<component::is_counter>().each(
        [&](entity_t counter_entity, const component::is_counter& counter) {
            auto owner_entity = registry.get<component::owner_component>(counter_entity).entity;
            if (owner_entity != actor_entity) {
                return;
            }
            counter_values.emplace_back(audit::counter_value_t{
                .counter = counter.counter_configuration.counter_key,
                .value = counter.value,
            });
        });
    return counter_values;
}

[[nodiscard]] std::unordered_map<actor::skill_t, audit::uncastable_skill_t> get_uncastable_skills(
    entity_t actor_entity,
    registry_t& registry) {
    std::unordered_map<std::string, audit::uncastable_skill_t> uncastable_skills;
    registry.view<component::is_skill, component::owner_component>().each(
        [&](entity_t skill_entity,
            const component::is_skill& is_skill,
            component::owner_component& owner_component) {
            if (owner_component.entity != actor_entity) {
                return;
            }
            auto skill_castability = utils::can_cast_skill(
                is_skill.skill_configuration.skill_key, actor_entity, registry);
            if (!skill_castability.can_cast) {
                auto cooldown_component =
                    registry.try_get<component::cooldown_component>(skill_entity);
                auto ammo = registry.try_get<component::ammo>(skill_entity);
                bool has_alacrity = registry.any_of<component::has_alacrity>(actor_entity);
                int remaining_cooldown = cooldown_component
                                             ? cooldown_component->duration[has_alacrity] -
                                                   cooldown_component->progress[has_alacrity]
                                             : 0;
                int remaining_ammo = ammo ? ammo->current_ammo : 0;
                uncastable_skills[is_skill.skill_configuration.skill_key] = {
                    .reason = skill_castability.reason,
                    .remaining_cooldown = remaining_cooldown,
                    .remaining_ammo = remaining_ammo,
                };
            }
        });
    return uncastable_skills;
}

[[maybe_unused]] std::map<std::basic_string<char>, std::map<actor::attribute_t, double>>
get_actor_attributes(registry_t& registry) {
    std::map<std::basic_string<char>, std::map<actor::attribute_t, double>> actor_attributes;
    registry.view<component::is_actor, component::relative_attributes>().each(
        [&](entity_t actor_entity, const component::relative_attributes& relative_attributes) {
            actor_attributes[utils::get_entity_name(actor_entity, registry)] =
                relative_attributes.entity_and_attribute_to_value_map.at(actor_entity);
        });
    return actor_attributes;
}

audit::tick_event_t create_tick_event(const decltype(audit::tick_event_t::event)& event,
                                      entity_t actor_entity,
                                      registry_t& registry) {
    auto bundle_ptr = registry.try_get<component::bundle_component>(actor_entity);
    auto weapon_set_ptr = registry.try_get<component::current_weapon_set>(actor_entity);
    return audit::tick_event_t{
        .time_ms = utils::get_current_tick(registry),
        .actor = utils::get_entity_name(actor_entity, registry),
        .event = event,
        .skill_cooldowns = get_skill_cooldowns(actor_entity, registry),
        .counter_values = get_counter_values(actor_entity, registry),
        .current_weapon_set = weapon_set_ptr ? weapon_set_ptr->set : actor::weapon_set::INVALID,
        .current_bundle = bundle_ptr ? bundle_ptr->name : "",
        .uncastable_skills = get_uncastable_skills(actor_entity, registry),
        //.actor_attributes = get_actor_attributes(registry),
    };
}

void audit_bundles(registry_t& registry) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    registry.view<component::equipped_bundle>().each(
        [&](entity_t actor_entity, const component::equipped_bundle& equipped_bundle) {
            audit_component.events.emplace_back(create_tick_event(
                audit::equipped_bundle_event_t{
                    .bundle = equipped_bundle.name,
                },
                actor_entity,
                registry));
        });
    registry.view<component::dropped_bundle>().each(
        [&](entity_t actor_entity, const component::dropped_bundle& dropped_bundle) {
            audit_component.events.emplace_back(create_tick_event(
                audit::dropped_bundle_event_t{
                    .bundle = dropped_bundle.name,
                },
                actor_entity,
                registry));
        });
}

void audit_actor_downstate(registry_t& registry) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    registry.view<component::is_downstate>().each([&](entity_t actor_entity) {
        audit_component.events.emplace_back(
            create_tick_event(audit::actor_downstate_event_t{}, actor_entity, registry));
    });
}

void audit_effect_expiration(registry_t& registry) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    registry.view<component::duration_expired>().each([&](entity_t entity) {
        auto actor_entity = utils::get_owner(entity, registry);
        auto source_actor = registry.get<component::source_actor>(entity);
        auto source_skill = registry.get<component::source_skill>(entity);
        auto effect = registry.any_of<component::is_effect>(entity)
                          ? nlohmann::json{registry.get<component::is_effect>(entity).effect}[0]
                                .get<std::string>()
                          : "";
        auto unique_effect =
            registry.any_of<component::is_unique_effect>(entity)
                ? registry.get<component::is_unique_effect>(entity).unique_effect.unique_effect_key
                : "";
        audit_component.events.emplace_back(create_tick_event(
            audit::effect_expired_event_t{
                .source_actor = utils::get_entity_name(source_actor.entity, registry),
                .source_skill = source_skill.skill,
                .effect = effect,
                .unique_effect = unique_effect,
            },
            actor_entity,
            registry));
    });
}

void audit_actor_created(registry_t& registry) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    registry.view<component::actor_created>().each([&](entity_t actor_entity) {
        audit_component.events.emplace_back(
            create_tick_event(audit::actor_created_event_t{}, actor_entity, registry));
    });
}

void audit_combat_stats_update(registry_t& registry) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    registry.view<component::combat_stats_updated, component::combat_stats>().each(
        [&](entity_t actor_entity, const component::combat_stats& combat_stats) {
            audit_component.events.emplace_back(create_tick_event(
                audit::combat_stats_update_event_t{
                    .updated_health = combat_stats.health,
                },
                actor_entity,
                registry));
        });
}

void audit_effect_applications(registry_t& registry) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    registry.view<component::incoming_effects_component>().each(
        [&](entity_t actor_entity,
            const component::incoming_effects_component& incoming_effects_component) {
            for (auto&& [source_entity, effect_application] :
                 incoming_effects_component.effect_applications) {
                auto actual_source_entity = utils::get_owner(source_entity, registry);
                auto source_actor = utils::get_entity_name(actual_source_entity, registry);
                auto effect = effect_application.effect == actor::effect_t::INVALID
                                  ? ""
                                  : nlohmann::json{effect_application.effect}[0].get<std::string>();
                auto& application_source_relative_attributes =
                    registry.get<component::relative_attributes>(actual_source_entity);
                int effective_duration = effect_application.effect == actor::effect_t::INVALID
                                             ? effect_application.base_duration_ms
                                             : utils::get_effective_effect_duration(
                                                   effect_application.base_duration_ms,
                                                   effect_application.effect,
                                                   application_source_relative_attributes,
                                                   actor_entity);
                audit_component.events.emplace_back(create_tick_event(
                    audit::effect_application_event_t{
                        .source_actor = source_actor,
                        .source_skill = effect_application.source_skill,
                        .effect = effect,
                        .unique_effect = effect_application.unique_effect.unique_effect_key,
                        .num_stacks = effect_application.num_stacks,
                        .duration_ms = effective_duration,
                    },
                    actor_entity,
                    registry));
            }
        });
}

void audit_skill_casts(registry_t& registry) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    registry.view<component::skills_actions_component>().each(
        [&](entity_t actor_entity,
            const component::skills_actions_component& casting_skills_component) {
            for (auto& casting_skill : casting_skills_component.skills) {
                if (casting_skill.strike_progress[0] + casting_skill.strike_progress[1] > 1 ||
                    casting_skill.pulse_progress[0] + casting_skill.pulse_progress[1] > 1) {
                    continue;
                }

                audit_component.events.emplace_back(create_tick_event(
                    audit::skill_cast_begin_event_t{
                        .skill = registry.get<component::is_skill>(casting_skill.skill_entity)
                                     .skill_configuration.skill_key,
                    },
                    actor_entity,
                    registry));
            }
        });

    registry.view<component::finished_casting_skills>().each(
        [&](entity_t actor_entity,
            const component::finished_casting_skills& finished_casting_skills) {
            for (auto finished_casting_skill_entity : finished_casting_skills.skill_entities) {
                audit_component.events.emplace_back(create_tick_event(
                    audit::skill_cast_end_event_t{
                        .skill = registry.get<component::is_skill>(finished_casting_skill_entity)
                                     .skill_configuration.skill_key,
                    },
                    actor_entity,
                    registry));
            }
        });
}

void audit_damage(registry_t& registry) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    registry.view<component::incoming_damage>().each(
        [&](entity_t actor_entity, const component::incoming_damage& incoming_damage) {
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
                auto skill_to_attribute_damage_to = [&]() {
                    if (incoming_damage_event.skill.empty()) {
                        return std::string{"unknown_skill"};
                    } else {
                        auto& skill_configuration =
                            utils::get_skill_configuration(incoming_damage_event.skill,
                                                           incoming_damage_event.source_entity,
                                                           registry);
                        return skill_configuration.attribute_damage_to_skill.empty()
                                   ? skill_configuration.skill_key
                                   : skill_configuration.attribute_damage_to_skill;
                    }
                }();
                audit_component.events.emplace_back(create_tick_event(
                    audit::damage_event_t{
                        .source_actor =
                            utils::get_entity_name(incoming_damage_event.source_entity, registry),
                        .source_skill = skill_to_attribute_damage_to,
                        .damage_type = damage_type,
                        .damage = incoming_damage_event.value,
                    },
                    actor_entity,
                    registry));
            }
        });
}

void audit(registry_t& registry) {
    auto& audit_configuration =
        registry.get<component::audit_component>(utils::get_singleton_entity()).audit_configuration;
    if (audit_configuration.audits_to_perform.contains(
            configuration::audit_t::audit_type_t::ACTOR_CREATED)) {
        audit_actor_created(registry);
    }
    if (audit_configuration.audits_to_perform.contains(
            configuration::audit_t::audit_type_t::SKILL_CASTS)) {
        audit_skill_casts(registry);
    }
    if (audit_configuration.audits_to_perform.contains(
            configuration::audit_t::audit_type_t::BUNDLES)) {
        audit_bundles(registry);
    }
    if (audit_configuration.audits_to_perform.contains(
            configuration::audit_t::audit_type_t::EFFECT_APPLICATIONS)) {
        audit_effect_applications(registry);
    }
    if (audit_configuration.audits_to_perform.contains(
            configuration::audit_t::audit_type_t::DAMAGE)) {
        audit_damage(registry);
    }
    if (audit_configuration.audits_to_perform.contains(
            configuration::audit_t::audit_type_t::COMBAT_STATS)) {
        audit_combat_stats_update(registry);
    }
    if (audit_configuration.audits_to_perform.contains(
            configuration::audit_t::audit_type_t::EFFECT_EXPIRATION)) {
        audit_effect_expiration(registry);
    }
    if (audit_configuration.audits_to_perform.contains(
            configuration::audit_t::audit_type_t::ACTOR_DOWNSTATE)) {
        audit_actor_downstate(registry);
    }
}

audit::report_t get_audit_report(registry_t& registry, int offset, const std::string& error) {
    audit::report_t audit_report;
    registry.view<component::audit_component>().each(
        [&](const component::audit_component& audit_component) {
            std::copy(audit_component.events.cbegin() + offset,
                      audit_component.events.cend(),
                      std::back_inserter(audit_report.tick_events));
        });
    if (!error.empty()) {
        audit_report.error = error;
    }
    audit_report.offset = offset;
    return audit_report;
}

}  // namespace gw2combat::system

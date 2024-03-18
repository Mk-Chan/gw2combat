#include "audit.hpp"

#include <component/temporal/has_quickness.hpp>

#include "utils/effect_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/skill_utils.hpp"

#include "system/attributes.hpp"

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
#include "component/encounter/encounter_configuration_component.hpp"
#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_conditional_skill_group.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/animation_component.hpp"
#include "component/temporal/cooldown_component.hpp"
#include "component/temporal/duration_component.hpp"
#include "component/temporal/has_alacrity.hpp"

namespace gw2combat::system {

audit::tick_event_t create_tick_event(const decltype(audit::tick_event_t::event)& event,
                                      entity_t actor_entity,
                                      registry_t& registry) {
    return audit::tick_event_t{
        .time_ms = utils::get_current_tick(registry),
        .actor = utils::get_entity_name(actor_entity, registry),
        .event = event,
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
                if (casting_skill.action_progress[0] + casting_skill.action_progress[1] > 1) {
                    continue;
                }
                auto& skill_configuration =
                    registry.get<component::is_skill>(casting_skill.skill_entity)
                        .skill_configuration;
                int cast_duration = registry.any_of<component::has_quickness>(actor_entity)
                                        ? skill_configuration.cast_duration[1]
                                        : skill_configuration.cast_duration[0];
                audit_component.events.emplace_back(create_tick_event(
                    audit::skill_cast_begin_event_t{
                        .skill = skill_configuration.skill_key,
                        .cast_duration = cast_duration,
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

void audit_afk(registry_t& registry) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    registry.view<component::is_afk>().each([&](entity_t actor_entity) {
        ++audit_component.afk_ticks_by_actor[utils::get_entity_name(actor_entity, registry)];
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
    audit_afk(registry);
}

[[nodiscard]] std::vector<audit::counter_value_t> get_counter_values(registry_t& registry) {
    // NOTE: Counter names are globally distinct, so we don't need to group them by actor.
    std::vector<audit::counter_value_t> counter_values;
    for (auto&& [counter_entity, is_counter] : registry.view<component::is_counter>().each()) {
        counter_values.emplace_back(audit::counter_value_t{
            .counter = is_counter.counter_configuration.counter_key,
            .value = is_counter.value,
        });
    }
    return counter_values;
}

std::map<std::string, std::vector<std::string>> get_castable_skills_by_actor(registry_t& registry) {
    std::map<std::string, std::vector<std::string>> castable_skills_by_actor;
    for (auto&& [actor_entity] :
         registry.view<component::is_actor>(entt::exclude<component::owner_component>).each()) {
        std::vector<std::string> actor_castable_skills;
        for (auto&& [skill_entity, is_skill, owner_component] :
             registry.view<component::is_skill, component::owner_component>().each()) {
            if (owner_component.entity != actor_entity) {
                continue;
            }

            auto skill_castability = utils::can_cast_skill(skill_entity, registry);
            if (skill_castability.can_cast && is_skill.skill_configuration.executable) {
                actor_castable_skills.emplace_back(is_skill.skill_configuration.skill_key);
            }
        }
        castable_skills_by_actor[utils::get_entity_name(actor_entity, registry)] =
            std::move(actor_castable_skills);
    }
    return castable_skills_by_actor;
}

[[nodiscard]] std::map<std::string, std::map<actor::skill_t, audit::uncastable_skill_t>>
get_uncastable_skills_by_actor(registry_t& registry) {
    std::map<std::string, std::map<actor::skill_t, audit::uncastable_skill_t>>
        uncastable_skills_by_actor;
    for (auto&& [actor_entity] :
         registry.view<component::is_actor>(entt::exclude<component::owner_component>).each()) {
        std::map<std::string, audit::uncastable_skill_t> actor_uncastable_skills;
        for (auto&& [skill_entity, is_skill, owner_component] :
             registry.view<component::is_skill, component::owner_component>().each()) {
            if (owner_component.entity != actor_entity) {
                continue;
            }

            auto skill_castability = utils::can_cast_skill(skill_entity, registry);
            if (skill_castability.can_cast) {
                continue;
            }
            auto cooldown_component = registry.try_get<component::cooldown_component>(skill_entity);
            auto ammo = registry.try_get<component::ammo>(skill_entity);
            int remaining_cooldown = 0;
            if (cooldown_component) {
                double no_alacrity_progress_pct =
                    cooldown_component->progress[0] * 100.0 / cooldown_component->duration[0];
                double alacrity_progress_pct =
                    cooldown_component->progress[1] * 100.0 / cooldown_component->duration[1];
                bool has_alacrity =
                    registry.any_of<component::has_alacrity>(owner_component.entity);
                remaining_cooldown = static_cast<int>(
                    cooldown_component->duration[has_alacrity] *
                    (1.0 - (alacrity_progress_pct + no_alacrity_progress_pct) / 100.0));
            }
            int remaining_ammo = ammo ? ammo->current_ammo : 0;
            actor_uncastable_skills[is_skill.skill_configuration.skill_key] = {
                .reason = skill_castability.reason,
                .remaining_cooldown = remaining_cooldown,
                .remaining_ammo = remaining_ammo,
            };

            // TODO: See if we can just get rid of this obsolete concept
            auto is_part_of_conditional_skill_group_ptr =
                registry.try_get<component::is_part_of_conditional_skill_group>(skill_entity);
            if (is_part_of_conditional_skill_group_ptr) {
                auto& conditional_skill_group_configuration =
                    registry
                        .get<component::is_conditional_skill_group>(
                            is_part_of_conditional_skill_group_ptr->conditional_skill_group_entity)
                        .conditional_skill_group_configuration;
                auto& conditional_skill_group_skill_key =
                    conditional_skill_group_configuration.skill_key;
                if (!actor_uncastable_skills.contains(conditional_skill_group_skill_key)) {
                    actor_uncastable_skills[conditional_skill_group_skill_key] = {
                        .reason = skill_castability.reason,
                        .remaining_cooldown = remaining_cooldown,
                        .remaining_ammo = remaining_ammo,
                    };
                }
            }
        }
        uncastable_skills_by_actor[utils::get_entity_name(actor_entity, registry)] =
            std::move(actor_uncastable_skills);
    }
    return uncastable_skills_by_actor;
}

[[nodiscard]] std::map<std::string, actor::weapon_set> get_current_weapon_set_by_actor(
    registry_t& registry) {
    std::map<std::string, actor::weapon_set> current_weapon_set_by_actor;
    for (auto&& [actor_entity, current_weapon_set] :
         registry.view<component::current_weapon_set>(entt::exclude<component::owner_component>)
             .each()) {
        current_weapon_set_by_actor[utils::get_entity_name(actor_entity, registry)] =
            current_weapon_set.set;
    }
    return current_weapon_set_by_actor;
}

[[nodiscard]] std::map<std::string, actor::bundle_t> get_current_bundle_by_actor(
    registry_t& registry) {
    std::map<std::string, actor::bundle_t> current_bundle_by_actor;
    for (auto&& [actor_entity, bundle_component] :
         registry.view<component::bundle_component>(entt::exclude<component::owner_component>)
             .each()) {
        current_bundle_by_actor[utils::get_entity_name(actor_entity, registry)] =
            bundle_component.name;
    }
    return current_bundle_by_actor;
}

[[maybe_unused]] std::map<std::string, std::map<std::string, audit::actor_effect_summary_t>>
get_effects_by_actor(registry_t& registry) {
    std::map<std::string, std::map<std::string, audit::actor_effect_summary_t>> effects_by_actor;
    for (auto&& [actor_entity] :
         registry.view<component::is_actor>(entt::exclude<component::owner_component>).each()) {
        std::vector<audit::actor_effect_t> actor_effects;
        for (auto&& [effect_entity, is_effect, owner_component] :
             registry.view<component::is_effect, component::owner_component>().each()) {
            if (owner_component.entity != actor_entity) {
                continue;
            }

            auto& duration_component = registry.get<component::duration_component>(effect_entity);
            auto& source_actor = registry.get<component::source_actor>(effect_entity);
            actor_effects.emplace_back(audit::actor_effect_t{
                .effect = is_effect.effect,
                .source_actor = utils::get_entity_name(source_actor.entity, registry),
                .remaining_duration = duration_component.duration - duration_component.progress,
            });
        }

        std::map<std::string, audit::actor_effect_summary_t> effect_summary;
        for (auto& effect : actor_effects) {
            auto& effect_summary_entry = effect_summary[nlohmann::json{effect.effect}[0]];
            effect_summary_entry.stacks++;
            if (effect.remaining_duration > effect_summary_entry.remaining_duration) {
                effect_summary_entry.remaining_duration = effect.remaining_duration;
            }
        }

        effects_by_actor[utils::get_entity_name(actor_entity, registry)] =
            std::move(effect_summary);
    }
    return effects_by_actor;
}

[[maybe_unused]] std::map<std::string, std::map<std::string, audit::actor_unique_effect_summary_t>>
get_unique_effects_by_actor(registry_t& registry) {
    std::map<std::string, std::map<std::string, audit::actor_unique_effect_summary_t>>
        unique_effects_by_actor;
    for (auto&& [actor_entity] :
         registry.view<component::is_actor>(entt::exclude<component::owner_component>).each()) {
        std::vector<audit::actor_unique_effect_t> actor_unique_effects;
        for (auto&& [unique_effect_entity, is_unique_effect, owner_component] :
             registry.view<component::is_unique_effect, component::owner_component>().each()) {
            if (owner_component.entity != actor_entity) {
                continue;
            }
            auto& duration_component =
                registry.get<component::duration_component>(unique_effect_entity);
            auto& source_actor = registry.get<component::source_actor>(unique_effect_entity);
            actor_unique_effects.emplace_back(audit::actor_unique_effect_t{
                .unique_effect = is_unique_effect.unique_effect.unique_effect_key,
                .source_actor = utils::get_entity_name(source_actor.entity, registry),
                .remaining_duration = duration_component.duration - duration_component.progress,
            });
        }

        std::map<actor::unique_effect_t, audit::actor_unique_effect_summary_t>
            unique_effect_summary;
        for (auto& unique_effect : actor_unique_effects) {
            auto& unique_effect_summary_entry = unique_effect_summary[unique_effect.unique_effect];
            unique_effect_summary_entry.stacks++;
            if (unique_effect.remaining_duration > unique_effect_summary_entry.remaining_duration) {
                unique_effect_summary_entry.remaining_duration = unique_effect.remaining_duration;
            }
        }

        unique_effects_by_actor[utils::get_entity_name(actor_entity, registry)] =
            std::move(unique_effect_summary);
    }
    return unique_effects_by_actor;
}

[[maybe_unused]] std::map<std::string, std::map<std::string, double>> get_attributes_by_actor(
    registry_t& registry) {
    system::calculate_relative_attributes(registry);
    std::map<std::string, std::map<std::string, double>> actor_attributes;
    for (auto&& [actor_entity, relative_attributes] :
         registry.view<component::relative_attributes>(entt::exclude<component::owner_component>)
             .each()) {
        if (relative_attributes.entity_and_attribute_to_value_map.empty()) {
            continue;
        }
        std::map<actor::attribute_t, double> entity_and_attribute_to_value_map =
            relative_attributes.entity_and_attribute_to_value_map.at(actor_entity);
        const std::string& actor_name = utils::get_entity_name(actor_entity, registry);
        std::transform(
            entity_and_attribute_to_value_map.cbegin(),
            entity_and_attribute_to_value_map.cend(),
            std::inserter(actor_attributes[actor_name], actor_attributes[actor_name].end()),
            [](const auto& pair) {
                return std::pair<std::string, double>{nlohmann::json{pair.first}[0], pair.second};
            });
    }
    registry.clear<component::relative_attributes>();
    return actor_attributes;
}

audit::report_t get_audit_report(registry_t& registry, int offset, const std::string& error) {
    auto& audit_component = registry.get<component::audit_component>(utils::get_singleton_entity());
    std::vector<audit::tick_event_t> tick_events;
    std::copy(audit_component.events.cbegin() + offset,
              audit_component.events.cend(),
              std::back_inserter(tick_events));
    std::optional<std::string> error_optional =
        error.empty() ? std::nullopt : std::make_optional(error);

    return audit::report_t{
        .offset = offset,
        .tick_events = tick_events,
        .error = error_optional,
        .counter_values = get_counter_values(registry),
        .castable_skills_by_actor = get_castable_skills_by_actor(registry),
        .uncastable_skills_by_actor = get_uncastable_skills_by_actor(registry),
        .current_weapon_set_by_actor = get_current_weapon_set_by_actor(registry),
        .current_bundle_by_actor = get_current_bundle_by_actor(registry),
        .effects_by_actor = get_effects_by_actor(registry),
        .unique_effects_by_actor = get_unique_effects_by_actor(registry),
        .attributes_by_actor = get_attributes_by_actor(registry),
        .afk_ticks_by_actor = audit_component.afk_ticks_by_actor,
    };
}

}  // namespace gw2combat::system

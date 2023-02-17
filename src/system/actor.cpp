#include "actor.hpp"

#include "component/actor/attribute_conversions_component.hpp"
#include "component/actor/attribute_modifiers_component.hpp"
#include "component/actor/casting_skill.hpp"
#include "component/actor/combat_stats.hpp"
#include "component/actor/did_weapon_swap.hpp"
#include "component/actor/effects_component.hpp"
#include "component/actor/finished_casting_skill.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/skills_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/hierarchy/owner_component.hpp"
#include "component/skill/skill_configuration_component.hpp"
#include "component/temporal/animation_component.hpp"

#include "utils/actor_utils.hpp"
#include "utils/condition_utils.hpp"
#include "utils/io_utils.hpp"
#include "utils/skill_utils.hpp"

namespace gw2combat::system {

void setup_combat_stats(registry_t& registry) {
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::owner_component, component::combat_stats>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            registry.emplace<component::combat_stats>(
                entity,
                component::combat_stats{
                    (int)static_attributes.attribute_value_map.at(actor::attribute_t::MAX_HEALTH)});
        });
}

void reset_combat_stats(registry_t& registry) {
    registry.view<component::static_attributes, component::combat_stats>().each(
        [&](const component::static_attributes& static_attributes,
            component::combat_stats& combat_stats) {
            combat_stats.health =
                (int)static_attributes.attribute_value_map.at(actor::attribute_t::MAX_HEALTH);
        });
}

void update_combat_stats(registry_t& registry) {
    bool health_updated = false;
    registry
        .view<component::combat_stats, component::incoming_damage>(
            entt::exclude<component::is_downstate>)
        .each([&](entity_t entity,
                  component::combat_stats& combat_stats,
                  const component::incoming_damage& incoming_damage) {
            combat_stats.health -= (int)incoming_damage.value;
            health_updated = true;

            if (combat_stats.health <= 0) {
                registry.emplace<component::is_downstate>(entity);
            }
        });
    if (health_updated) {
        utils::log_component<component::combat_stats>(registry);
    }
}

void calculate_relative_attributes(registry_t& registry) {
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::owner_component, component::relative_attributes>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            auto& relative_attributes = registry.emplace<component::relative_attributes>(entity);
            registry
                .view<component::is_actor, component::static_attributes>(
                    entt::exclude<component::owner_component>)
                .each([&](entity_t other_entity, const component::static_attributes&) {
                    for (auto& [attribute, value] : static_attributes.attribute_value_map) {
                        relative_attributes.set(other_entity, attribute, value);
                    }
                });
        });

    registry.view<component::owner_component, component::attribute_modifiers_component>().each(
        [&](entity_t,
            const component::owner_component& owner_actor,
            const component::attribute_modifiers_component& attribute_modifiers_component) {
            auto source_entity = owner_actor.entity;
            auto& relative_attributes = registry.get<component::relative_attributes>(source_entity);
            registry.view<component::relative_attributes>().each(
                [&](entity_t other_entity, const component::relative_attributes&) {
                    for (auto& attribute_modifier :
                         attribute_modifiers_component.attribute_modifiers) {
                        if (utils::conditions_satisfied(attribute_modifier.condition,
                                                        source_entity,
                                                        other_entity,
                                                        registry)) {
                            relative_attributes.set(
                                other_entity,
                                attribute_modifier.attribute,
                                relative_attributes.get(other_entity,
                                                        attribute_modifier.attribute) *
                                        attribute_modifier.multiplier +
                                    attribute_modifier.addend);
                            // spdlog::info("{} {} modifier {} against {}",
                            //              utils::get_entity_name(source_entity, registry),
                            //              utils::get_entity_name(entity, registry),
                            //              utils::to_string(attribute_modifier),
                            //              utils::get_entity_name(other_entity, registry));
                        }
                    }
                });
        });

    registry.view<component::owner_component, component::attribute_conversions_component>().each(
        [&](entity_t,
            const component::owner_component& owner_actor,
            const component::attribute_conversions_component& attribute_conversions_component) {
            auto source_entity = owner_actor.entity;
            auto& relative_attributes = registry.get<component::relative_attributes>(source_entity);
            registry.view<component::relative_attributes>().each(
                [&](entity_t other_entity, const component::relative_attributes&) {
                    std::unordered_map<actor::attribute_t, double> attribute_conversion_bonuses;
                    for (auto& attribute_conversion :
                         attribute_conversions_component.attribute_conversions) {
                        if (utils::conditions_satisfied(attribute_conversion.condition,
                                                        source_entity,
                                                        other_entity,
                                                        registry)) {
                            attribute_conversion_bonuses[attribute_conversion.to] +=
                                (int)(attribute_conversion_bonuses[attribute_conversion.from] *
                                          attribute_conversion.multiplier +
                                      attribute_conversion.addend);
                        }
                    }

                    for (auto&& [attribute, bonus] : attribute_conversion_bonuses) {
                        relative_attributes.set(
                            other_entity,
                            attribute,
                            relative_attributes.get(other_entity, attribute) + bonus);
                        // spdlog::info(
                        //     "{} {} conversion {} against {}",
                        //     utils::get_entity_name(source_entity, registry),
                        //     utils::get_entity_name(entity, registry),
                        //     utils::to_string(relative_attributes.get(other_entity, attribute)),
                        //     utils::get_entity_name(other_entity, registry));
                    }
                });
        });
}

void perform_rotations(registry_t& registry) {
    registry
        .view<component::rotation_component>(
            entt::exclude<component::animation_component, component::no_more_rotation>)
        .each([&](entity_t entity, component::rotation_component& rotation_component) {
            auto current_tick = utils::get_current_tick(registry);

            if (rotation_component.current_idx >=
                (int)rotation_component.rotation.skill_casts.size()) {
                if (rotation_component.repeat) {
                    rotation_component.current_idx = 0;
                    rotation_component.tick_offset = current_tick;
                } else {
                    registry.emplace<component::no_more_rotation>(entity);
                    spdlog::info("[{}] {} has no more rotation",
                                 utils::get_current_tick(registry),
                                 utils::get_entity_name(entity, registry));
                    return;
                }
            }

            auto& next_skill_cast =
                rotation_component.rotation.skill_casts[rotation_component.current_idx];

            // Make sure this skill can only be cast at or after the time specified in the rotation
            // configuration
            if (current_tick < next_skill_cast.cast_time_ms + rotation_component.tick_offset) {
                return;
            }

            spdlog::info("[{}] {} casting skill {}",
                         utils::get_current_tick(registry),
                         utils::get_entity_name(entity, registry),
                         utils::to_string(next_skill_cast.skill));

            auto skill_entity =
                registry.get<component::skills_component>(entity).find_by(next_skill_cast.skill);
            auto& skill_cast_duration =
                registry.get<component::skill_configuration_component>(skill_entity)
                    .skill_configuration.cast_duration;
            registry.emplace<component::animation_component>(
                entity, component::animation_component{skill_cast_duration, {0, 0}});
            registry.emplace<component::casting_skill>(
                entity, component::casting_skill{skill_entity, 0, 0});

            rotation_component.current_idx += 1;
        });
}

void perform_skills(registry_t& registry) {
    registry
        .view<component::animation_component, component::casting_skill>(
            entt::exclude<component::finished_casting_skill>)
        .each([&](entity_t entity,
                  const component::animation_component& animation_component,
                  component::casting_skill& casting_skill) {
            double no_quickness_progress_pct = animation_component.duration[0] == 0
                                                   ? 100
                                                   : animation_component.progress[0] * 100 /
                                                         (double)animation_component.duration[0];
            double quickness_progress_pct = animation_component.duration[1] == 0
                                                ? 100
                                                : animation_component.progress[1] * 100 /
                                                      (double)animation_component.duration[1];

            auto& skill_configuration =
                registry.get<component::skill_configuration_component>(casting_skill.skill_entity)
                    .skill_configuration;

            double effective_progress_pct = no_quickness_progress_pct + quickness_progress_pct;
            int effective_tick =
                (int)((double)animation_component.duration[0] * effective_progress_pct / 100.0);

            while (casting_skill.next_pulse_idx <
                       (int)skill_configuration.pulse_on_tick_list[0].size() &&
                   effective_tick >=
                       skill_configuration.pulse_on_tick_list[0][casting_skill.next_pulse_idx]) {
                auto& outgoing_effects_component =
                    registry.get_or_emplace<component::outgoing_effects_component>(entity);
                for (auto& effect_application : skill_configuration.on_pulse_effect_applications) {
                    outgoing_effects_component.effect_applications.emplace_back(effect_application);
                }
                ++casting_skill.next_pulse_idx;
            }

            while (casting_skill.next_strike_idx <
                       (int)skill_configuration.strike_on_tick_list[0].size() &&
                   effective_tick >=
                       skill_configuration.strike_on_tick_list[0][casting_skill.next_strike_idx]) {
                auto& outgoing_strikes_component =
                    registry.get_or_emplace<component::outgoing_strikes_component>(entity);
                auto this_strike = component::strike_t{casting_skill.skill_entity,
                                                       skill_configuration.num_targets};
                outgoing_strikes_component.strikes.emplace_back(this_strike);
                ++casting_skill.next_strike_idx;
            }

            if (effective_progress_pct >= 100) {
                spdlog::info("[{}] {}: finished casting skill {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(entity, registry),
                             utils::to_string(skill_configuration.skill_key));
                registry.emplace<component::finished_casting_skill>(
                    entity, component::finished_casting_skill{casting_skill.skill_entity});
                utils::put_skill_on_cooldown(
                    utils::get_owner(entity, registry), skill_configuration.skill_key, registry);
            }
        });

    registry.view<component::finished_casting_skill>().each(
        [&](entity_t entity, const component::finished_casting_skill& finished_casting_skill) {
            auto& skill_configuration = registry
                                            .get<component::skill_configuration_component>(
                                                finished_casting_skill.skill_entity)
                                            .skill_configuration;
            if (skill_configuration.skill_key == "Weapon Swap") {
                if (registry.any_of<component::bundle_component>(entity)) {
                    registry.remove<component::bundle_component>(entity);
                    return;
                }
                if (!registry.any_of<component::current_weapon_set>(entity)) {
                    throw std::runtime_error("no equipped_weapon_set on entity");
                }

                registry.emplace<component::did_weapon_swap>(entity);

                auto current_set = registry.get<component::current_weapon_set>(entity).set;
                if (current_set == actor::weapon_set::SET_1) {
                    registry.replace<component::current_weapon_set>(
                        entity, component::current_weapon_set{actor::weapon_set::SET_2});
                } else {
                    registry.replace<component::current_weapon_set>(
                        entity, component::current_weapon_set{actor::weapon_set::SET_1});
                }
            }

            auto& skills_component = registry.get<component::skills_component>(entity);
            std::vector<configuration::skill_t> child_skills;
            for (auto&& child_skill_key : skill_configuration.child_skill_keys) {
                auto& child_skill_configuration =
                    registry
                        .get<component::skill_configuration_component>(
                            skills_component.find_by(child_skill_key))
                        .skill_configuration;
                child_skills.emplace_back(child_skill_configuration);
            }
            utils::enqueue_child_skills(entity,
                                        "Temporary " + skill_configuration.skill_key + " Entity",
                                        child_skills,
                                        registry);
        });
}

void cleanup_finished_casting_skills(registry_t& registry) {
    registry.view<component::finished_casting_skill>().each(
        [&](entity_t entity, const component::finished_casting_skill& finished_casting_skill) {
            registry.remove<component::casting_skill>(entity);
            registry.destroy(finished_casting_skill.skill_entity);
            registry.remove<component::finished_casting_skill>(entity);
        });
}

}  // namespace gw2combat::system

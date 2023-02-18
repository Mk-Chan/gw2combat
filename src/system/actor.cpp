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

#include "component/skill/skill_trigger_lock.hpp"
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

    std::map<std::tuple<actor::unique_effect_t, entity_t, entity_t>, int>
        modifier_unique_effect_and_owner_entity_and_other_entity_to_occurrences_map;
    std::map<std::tuple<actor::effect_t, entity_t, entity_t>, int>
        modifier_effect_and_owner_entity_and_other_entity_to_occurrences_map;
    registry.view<component::owner_component, component::attribute_modifiers_component>().each(
        [&](entity_t modifier_entity,
            const component::owner_component& owner_actor,
            const component::attribute_modifiers_component& attribute_modifiers_component) {
            auto unique_effect_modifier_ptr =
                registry.try_get<component::is_unique_effect>(modifier_entity);
            auto effect_modifier_ptr = registry.try_get<component::is_effect>(modifier_entity);
            auto owner_entity = owner_actor.entity;
            auto& relative_attributes = registry.get<component::relative_attributes>(owner_entity);
            registry.view<component::relative_attributes>().each([&](entity_t other_entity,
                                                                     const component::
                                                                         relative_attributes&) {
                if (unique_effect_modifier_ptr) {
                    if (modifier_unique_effect_and_owner_entity_and_other_entity_to_occurrences_map
                            [std::make_tuple(
                                unique_effect_modifier_ptr->unique_effect.unique_effect_key,
                                owner_entity,
                                other_entity)] >=
                        unique_effect_modifier_ptr->unique_effect.max_considered_stacks) {
                        return;
                    } else {
                        ++modifier_unique_effect_and_owner_entity_and_other_entity_to_occurrences_map
                            [std::make_tuple(
                                unique_effect_modifier_ptr->unique_effect.unique_effect_key,
                                owner_entity,
                                other_entity)];
                    }
                }
                if (effect_modifier_ptr) {
                    if (modifier_effect_and_owner_entity_and_other_entity_to_occurrences_map
                            [std::make_tuple(
                                effect_modifier_ptr->effect, owner_entity, other_entity)] >=
                        utils::get_max_considered_stacks_of_effect_type(
                            effect_modifier_ptr->effect)) {
                        return;
                    } else {
                        ++modifier_effect_and_owner_entity_and_other_entity_to_occurrences_map
                            [std::make_tuple(
                                effect_modifier_ptr->effect, owner_entity, other_entity)];
                    }
                }
                for (auto& attribute_modifier : attribute_modifiers_component.attribute_modifiers) {
                    if (utils::conditions_satisfied(
                            attribute_modifier.condition, owner_entity, other_entity, registry)) {
                        relative_attributes.set(
                            other_entity,
                            attribute_modifier.attribute,
                            relative_attributes.get(other_entity, attribute_modifier.attribute) *
                                    attribute_modifier.multiplier +
                                attribute_modifier.addend);
                        // spdlog::info("{}:{} source {} modifier {} final {}",
                        //              utils::get_entity_name(owner_entity, registry),
                        //              utils::get_entity_name(other_entity, registry),
                        //              utils::get_entity_name(modifier_entity, registry),
                        //              utils::to_string(attribute_modifier),
                        //              relative_attributes.get(other_entity,
                        //                                      attribute_modifier.attribute));
                    }
                }
            });
        });

    std::map<std::tuple<actor::unique_effect_t, entity_t, entity_t>, int>
        conversion_unique_effect_and_owner_entity_and_other_entity_to_occurrences_map;
    std::map<std::tuple<actor::effect_t, entity_t, entity_t>, int>
        conversion_effect_and_owner_entity_and_other_entity_to_occurrences_map;
    registry.view<component::relative_attributes>().each([&](entity_t other_entity,
                                                             const component::
                                                                 relative_attributes&) {
        std::map<std::tuple<entity_t, actor::attribute_t>, double>
            owner_entity_to_attribute_conversion_bonuses;
        registry.view<component::owner_component, component::attribute_conversions_component>()
            .each([&](entity_t conversion_entity,
                      const component::owner_component& owner_actor,
                      const component::attribute_conversions_component&
                          attribute_conversions_component) {
                auto unique_effect_conversion_ptr =
                    registry.try_get<component::is_unique_effect>(conversion_entity);
                auto effect_conversion_ptr =
                    registry.try_get<component::is_effect>(conversion_entity);
                auto owner_entity = owner_actor.entity;
                auto& relative_attributes =
                    registry.get<component::relative_attributes>(owner_entity);
                if (unique_effect_conversion_ptr) {
                    if (conversion_unique_effect_and_owner_entity_and_other_entity_to_occurrences_map
                            [std::make_tuple(
                                unique_effect_conversion_ptr->unique_effect.unique_effect_key,
                                owner_entity,
                                other_entity)] >=
                        unique_effect_conversion_ptr->unique_effect.max_considered_stacks) {
                        return;
                    } else {
                        ++conversion_unique_effect_and_owner_entity_and_other_entity_to_occurrences_map
                            [std::make_tuple(
                                unique_effect_conversion_ptr->unique_effect.unique_effect_key,
                                owner_entity,
                                other_entity)];
                    }
                }
                if (effect_conversion_ptr) {
                    if (conversion_effect_and_owner_entity_and_other_entity_to_occurrences_map
                            [std::make_tuple(
                                effect_conversion_ptr->effect, owner_entity, other_entity)] >=
                        utils::get_max_considered_stacks_of_effect_type(
                            effect_conversion_ptr->effect)) {
                        return;
                    } else {
                        ++conversion_effect_and_owner_entity_and_other_entity_to_occurrences_map
                            [std::make_tuple(
                                effect_conversion_ptr->effect, owner_entity, other_entity)];
                    }
                }
                for (auto& attribute_conversion :
                     attribute_conversions_component.attribute_conversions) {
                    if (utils::conditions_satisfied(
                            attribute_conversion.condition, owner_entity, other_entity, registry)) {
                        owner_entity_to_attribute_conversion_bonuses[std::make_tuple(
                            owner_entity, attribute_conversion.to)] +=
                            (int)((relative_attributes.get(other_entity,
                                                           attribute_conversion.from) *
                                   attribute_conversion.multiplier) +
                                  attribute_conversion.addend);
                    }
                }
            });

        for (auto&& [owner_entity_attribute_tuple, bonus] :
             owner_entity_to_attribute_conversion_bonuses) {
            auto& [owner_entity, attribute] = owner_entity_attribute_tuple;
            auto& relative_attributes = registry.get<component::relative_attributes>(owner_entity);
            relative_attributes.set(
                other_entity, attribute, relative_attributes.get(other_entity, attribute) + bonus);
            // spdlog::info("{}:{} source converting {} bonus {} final {}",
            //              utils::get_entity_name(owner_entity, registry),
            //              utils::get_entity_name(other_entity, registry),
            //              utils::to_string(attribute),
            //              bonus,
            //              utils::to_string(relative_attributes.get(other_entity, attribute)));
        }
    });

    registry.view<component::relative_attributes>().each(
        [&](entity_t, component::relative_attributes& relative_attributes) {
            registry.view<component::relative_attributes>().each(
                [&](entity_t other_entity, const component::relative_attributes&) {
                    double precision =
                        relative_attributes.get(other_entity, actor::attribute_t::PRECISION);
                    relative_attributes.set(
                        other_entity,
                        actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER,
                        relative_attributes.get(other_entity,
                                                actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER) +
                            (precision - 895) / 2100.0);

                    double ferocity =
                        relative_attributes.get(other_entity, actor::attribute_t::FEROCITY);
                    relative_attributes.set(
                        other_entity,
                        actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER,
                        relative_attributes.get(other_entity,
                                                actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER) +
                            ferocity / 1500.0);
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

            auto skill_entity =
                registry.get<component::skills_component>(entity).find_by(next_skill_cast.skill);

            if (!registry.any_of<component::skill_trigger_lock>(skill_entity)) {
                registry.emplace<component::skill_trigger_lock>(skill_entity);

                auto& skill_cast_duration =
                    registry.get<component::skill_configuration_component>(skill_entity)
                        .skill_configuration.cast_duration;
                registry.emplace<component::animation_component>(
                    entity, component::animation_component{skill_cast_duration, {0, 0}});
                registry.emplace<component::casting_skill>(
                    entity, component::casting_skill{skill_entity, 0, 0});

                spdlog::info("[{}] {} casting skill {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(entity, registry),
                             utils::to_string(next_skill_cast.skill));
            }

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
                registry.emplace<component::finished_casting_skill>(
                    entity, component::finished_casting_skill{casting_skill.skill_entity});
                utils::put_skill_on_cooldown(
                    utils::get_owner(entity, registry), skill_configuration.skill_key, registry);
                spdlog::info("[{}] {}: finished casting skill {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(entity, registry),
                             utils::to_string(skill_configuration.skill_key));
            }
        });

    registry.view<component::finished_casting_skill>().each(
        [&](entity_t entity, const component::finished_casting_skill& finished_casting_skill) {
            registry.remove<component::skill_trigger_lock>(finished_casting_skill.skill_entity);
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
        [&](entity_t entity, const component::finished_casting_skill&) {
            registry.remove<component::casting_skill>(entity);
            registry.remove<component::finished_casting_skill>(entity);
        });
}

void destroy_actors_with_no_rotation(registry_t& registry) {
    registry.view<component::destroy_after_rotation, component::no_more_rotation>().each(
        [&](entity_t entity) { registry.destroy(entity); });
}

}  // namespace gw2combat::system

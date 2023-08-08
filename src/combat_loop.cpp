#include "combat_loop.hpp"

#include "mru_cache.hpp"

#include "component/actor/begun_casting_skills.hpp"
#include "component/actor/combat_stats.hpp"
#include "component/actor/finished_casting_skills.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/skills_actions_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/effect/is_skill_trigger.hpp"
#include "component/equipment/bundle.hpp"
#include "component/lifecycle/destroy_entity.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/animation_component.hpp"
#include "component/temporal/cooldown_component.hpp"
#include "component/temporal/duration_component.hpp"

#include "system/actor.hpp"
#include "system/apply_strikes_and_effects.hpp"
#include "system/attributes.hpp"
#include "system/audit.hpp"
#include "system/dispatch_strikes_and_effects.hpp"
#include "system/effects.hpp"
#include "system/encounter.hpp"
#include "system/rotation.hpp"
#include "system/temporal.hpp"

#include "utils/condition_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/registry_utils.hpp"
#include "utils/side_effect_utils.hpp"

namespace gw2combat {

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::actor_created,
                   component::equipped_bundle,
                   component::dropped_bundle,
                   component::relative_attributes,
                   component::incoming_damage,
                   component::begun_casting_skills,
                   component::animation_expired,
                   component::cooldown_expired,
                   component::duration_expired,
                   component::ammo_gained,
                   component::outgoing_strikes_component,
                   component::outgoing_effects_component,
                   component::incoming_strikes_component,
                   component::incoming_effects_component,
                   component::incoming_damage,
                   component::combat_stats_updated>();
    registry.view<component::is_skill_trigger>().each(
        [&](component::is_skill_trigger& is_skill_trigger) {
            is_skill_trigger.already_triggered = false;
        });
    registry.view<component::finished_casting_skills>().each(
        [&](component::finished_casting_skills& finished_casting_skills) {
            finished_casting_skills.skill_entities.clear();
        });
}

void try_clean_entity(registry_t& registry, entity_t entity, entity_t owner_entity) {
    if (!registry.valid(owner_entity)) {
        registry.destroy(entity);
    } else if (registry.any_of<component::owner_component>(owner_entity)) {
        auto owners_owner_entity = registry.get<component::owner_component>(owner_entity).entity;
        try_clean_entity(registry, entity, owners_owner_entity);
    }
}

void clean_children_of_destroyed_owners(registry_t& registry) {
    registry.view<component::owner_component>().each(
        [&](entity_t entity, const component::owner_component& owner_component) {
            try_clean_entity(registry, entity, owner_component.entity);
        });
}

void destroy_marked_entities(registry_t& registry) {
    bool entity_was_destroyed = false;
    registry.view<component::destroy_entity>().each([&](entity_t entity) {
        registry.destroy(entity);
        entity_was_destroyed = true;
    });
    if (entity_was_destroyed) {
        clean_children_of_destroyed_owners(registry);
    }
}

void tick(registry_t& registry) {
    system::setup_combat_stats(registry);

    system::perform_rotations(registry);

    system::progress_animations(registry);
    system::progress_casting_skills(registry);
    system::progress_cooldowns(registry);
    system::progress_durations(registry);

    system::perform_skills(registry);

    registry.view<component::is_skill, component::ammo_gained>().each(
        [&](entity_t skill_entity, const component::is_skill& is_skill) {
            auto actor_entity = utils::get_owner(skill_entity, registry);
            auto side_effect_condition_fn = [&](const configuration::condition_t& condition) {
                return utils::on_ammo_gain_conditions_satisfied(
                    condition, actor_entity, is_skill.skill_configuration, registry);
            };
            utils::apply_side_effects(registry, actor_entity, side_effect_condition_fn);
        });
    registry.view<component::begun_casting_skills>().each(
        [&](entity_t actor_entity, component::begun_casting_skills& begun_casting_skills) {
            for (auto casting_skill_entity : begun_casting_skills.skill_entities) {
                auto& skill_configuration =
                    registry.get<component::is_skill>(casting_skill_entity).skill_configuration;
                auto side_effect_condition_fn = [&](const configuration::condition_t& condition) {
                    return utils::on_begun_casting_conditions_satisfied(
                        condition, actor_entity, skill_configuration, registry);
                };
                utils::apply_side_effects(registry, actor_entity, side_effect_condition_fn);
            }
        });
    registry.view<component::is_actor>(entt::exclude<component::owner_component>)
        .each([&](entity_t actor_entity) {
            auto side_effect_condition_fn = [&](const configuration::condition_t& condition) {
                return utils::independent_conditions_satisfied(
                           condition, actor_entity, std::nullopt, registry)
                    .satisfied;
            };
            utils::apply_side_effects(registry, actor_entity, side_effect_condition_fn);
        });

    system::dispatch_strikes(registry);

    if (!registry.view<component::incoming_strikes_component>().empty()) {
        system::calculate_relative_attributes(registry);
    }

    system::apply_strikes(registry);
    system::dispatch_effects(registry);

    if (!registry.view<component::incoming_effects_component>().empty()) {
        system::calculate_relative_attributes(registry);
    }

    system::apply_effects(registry);

    system::buffer_damage_for_effects_with_no_duration(registry);
    if (tick_t current_tick = utils::get_current_tick(registry); current_tick % 1000 == 0) {
        system::buffer_condition_damage(registry);
        system::apply_condition_damage(registry);
    }

    system::update_combat_stats(registry);

    system::cleanup_expired_components(registry);
    system::destroy_actors_with_no_rotation(registry);

    system::audit(registry);

    system::cleanup_skill_actions(registry);
    destroy_marked_entities(registry);
    clear_temporary_components(registry);
}

mru_cache_t<registry_t>::key_type convert_encounter_to_cache_key(
    const configuration::encounter_t& encounter) {
    configuration::encounter_t normalized_encounter{encounter};
    normalized_encounter.audit_offset = 0;
    return mru_cache_t<registry_t>::djb2_hash(utils::to_string(normalized_encounter));
}

bool continue_combat_loop(registry_t& registry, const configuration::encounter_t& encounter) {
    for (auto entity : registry.view<component::is_actor>()) {
        if (registry.any_of<component::is_downstate>(entity)) {
            spdlog::info("[{}] {} is downstate",
                         utils::get_current_tick(registry),
                         utils::get_entity_name(entity, registry));
            return false;
        }
    }

    for (auto& termination_condition : encounter.termination_conditions) {
        if (termination_condition.type ==
            configuration::termination_condition_t::type_t::ROTATION) {
            bool everyone_out_of_rotation = true;
            for (auto entity : registry.view<component::is_actor>()) {
                if (utils::get_entity_name(entity, registry) != termination_condition.actor ||
                    !registry.any_of<component::rotation_component>(entity)) {
                    continue;
                }
                if (registry.any_of<component::skills_actions_component,
                                    component::finished_skills_actions_component>(entity)) {
                    everyone_out_of_rotation = false;
                    break;
                }
                if (!registry.any_of<component::no_more_rotation>(entity)) {
                    everyone_out_of_rotation = false;
                    break;
                }
            }
            if (everyone_out_of_rotation) {
                return false;
            }
        } else if (termination_condition.type ==
                   configuration::termination_condition_t::type_t::DAMAGE) {
            bool someone_took_required_damage = false;
            for (auto&& [entity, static_attributes, combat_stats] :
                 registry
                     .view<component::is_actor,
                           component::static_attributes,
                           component::combat_stats>()
                     .each()) {
                if (utils::get_entity_name(entity, registry) != termination_condition.actor) {
                    continue;
                }
                int max_health = utils::round_to_nearest_even(
                    static_attributes.attribute_value_map[actor::attribute_t::MAX_HEALTH]);
                int current_health = combat_stats.health;
                if (max_health - current_health >= termination_condition.damage) {
                    someone_took_required_damage = true;
                    break;
                }
            }
            if (someone_took_required_damage) {
                return false;
            }
        } else if (termination_condition.type ==
                   configuration::termination_condition_t::type_t::TIME) {
            if (utils::get_current_tick(registry) >= termination_condition.time) {
                return false;
            }
        }
    }
    return true;
}

std::string combat_loop(const configuration::encounter_t& encounter, bool enable_caching) {
    auto& registry_cache = mru_cache_t<registry_t>::instance();

    registry_t registry;
    if (enable_caching) {
        auto actor = encounter.actors[0];
        size_t max_depth = actor.rotation.skill_casts.size();

        bool is_cache_miss = true;
        configuration::encounter_t current_encounter{encounter};
        for (size_t depth = 0; depth < max_depth && is_cache_miss; ++depth) {
            if (depth > 0) {
                current_encounter.actors[0].rotation.skill_casts.pop_back();
            }

            auto cache_key = convert_encounter_to_cache_key(current_encounter);
            if (registry_cache.contains(cache_key)) {
                registry.clear();
                utils::copy_registry(registry_cache.get(cache_key), registry);
                is_cache_miss = false;
                break;
            }
        }
        if (is_cache_miss) {
            registry.ctx().emplace<tick_t>(0);
            system::setup_encounter(registry, encounter);
        } else {
            for (auto&& [actor_entity] :
                 registry.view<component::is_actor>(entt::exclude<component::owner_component>)
                     .each()) {
                if (utils::get_entity_name(actor_entity, registry) == actor.name) {
                    registry.remove<component::no_more_rotation>(actor_entity);
                    actor::rotation_t converted_rotation{};
                    int offset = 0;
                    bool first = true;
                    for (auto&& skill_cast : actor.rotation.skill_casts) {
                        if (first) {
                            offset = std::min(skill_cast.cast_time_ms, 0);
                            first = false;
                        }
                        converted_rotation.skill_casts.emplace_back(actor::skill_cast_t{
                            skill_cast.skill, (tick_t)(skill_cast.cast_time_ms - offset)});
                    }
                    auto& existing_rotation_component =
                        registry.get<component::rotation_component>(actor_entity);
                    registry.replace<component::rotation_component>(
                        actor_entity,
                        component::rotation_component{converted_rotation,
                                                      existing_rotation_component.current_idx,
                                                      existing_rotation_component.tick_offset,
                                                      actor.rotation.repeat});
                    break;
                }
            }
        }
    } else {
        registry.ctx().emplace<tick_t>(0);
        system::setup_encounter(registry, encounter);
    }

    std::string result;
    try {
        while (continue_combat_loop(registry, encounter)) {
            registry.ctx().get<tick_t>() += 1;
            tick(registry);
        }
    } catch (std::exception& e) {
        spdlog::error("Exception: {}", e.what());
        return utils::to_string(
            system::get_audit_report(registry, encounter.audit_offset, e.what()));
    }

    result = utils::to_string(system::get_audit_report(registry, encounter.audit_offset));
    auto cache_key = convert_encounter_to_cache_key(encounter);
    if (!registry_cache.contains(cache_key)) {
        registry_cache.put(convert_encounter_to_cache_key(encounter), std::move(registry));
    }
    return result;
}

}  // namespace gw2combat

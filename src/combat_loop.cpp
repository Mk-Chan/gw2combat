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
#include "component/actor/skills_ticks_tracker_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/effect/is_skill_trigger.hpp"
#include "component/encounter/encounter_configuration_component.hpp"
#include "component/equipment/bundle.hpp"
#include "component/lifecycle/destroy_entity.hpp"
#include "component/skill/ammo.hpp"
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
#include "system/hooks.hpp"
#include "system/rotation.hpp"
#include "system/skill.hpp"
#include "system/temporal.hpp"

#include "utils/condition_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/registry_utils.hpp"
#include "utils/side_effect_utils.hpp"

namespace gw2combat {

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::actor_created,
                   component::already_performed_rotation,
                   component::already_performed_animation,
                   component::already_finished_casting_skill,
                   component::is_afk,
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

void mark_afk_actors(registry_t& registry) {
    registry
        .view<component::is_actor>(
            entt::exclude<component::owner_component, component::animation_component>)
        .each([&](entity_t actor_entity) { registry.emplace<component::is_afk>(actor_entity); });
}

void tick(registry_t& registry) {
    auto& encounter =
        registry.get<component::encounter_configuration_component>(utils::get_singleton_entity())
            .encounter;
    system::setup_combat_stats(registry);

    while (true) {
        bool repeat = false;
        repeat |= system::perform_rotations(registry);
        repeat |= system::progress_animations(registry);
        if (!repeat) {
            break;
        }
    }

    mark_afk_actors(registry);

    system::progress_casting_skill_ticks(registry);
    system::progress_casting_skills(registry);
    system::progress_cooldowns(registry);
    system::progress_durations(registry);

    system::perform_skill_ticks(registry);
    system::perform_skills(registry);

    system::on_ammo_gained_hooks(registry);
    system::on_begun_casting_skills_hooks(registry);
    system::on_every_tick_hooks(registry);

    if (!registry.view<component::outgoing_strikes_component>().empty()) {
        system::calculate_relative_attributes(registry);
    }

    system::dispatch_strikes(registry);
    system::apply_strikes(registry);
    system::on_strike_hooks(registry);

    if (!registry.view<component::outgoing_effects_component>().empty()) {
        system::calculate_relative_attributes(registry);
    }

    system::dispatch_effects(registry);
    system::apply_effects(registry);

    system::buffer_damage_for_effects_with_no_duration(registry);
    if (tick_t current_tick = utils::get_current_tick(registry);
        (current_tick + encounter.condition_tick_offset) % 1000 == 0) {
        system::buffer_condition_damage(registry);
        system::apply_condition_damage(registry);
    }

    system::update_combat_stats(registry);

    system::cleanup_expired_components(registry);
    system::destroy_actors_with_no_rotation(registry);

    system::audit(registry);

    system::cleanup_skill_ticks_tracker(registry);
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
            bool actors_out_of_rotation = true;
            for (auto entity : registry.view<component::is_actor>()) {
                if (!termination_condition.actor.empty() &&
                    utils::get_entity_name(entity, registry) != termination_condition.actor) {
                    continue;
                }
                if (!registry.any_of<component::rotation_component>(entity)) {
                    continue;
                }
                if (!registry.any_of<component::no_more_rotation>(entity) ||
                    registry.any_of<component::animation_component>(entity)) {
                    actors_out_of_rotation = false;
                    break;
                }
            }
            if (actors_out_of_rotation) {
                return false;
            }
        } else if (termination_condition.type ==
                   configuration::termination_condition_t::type_t::ACTIVE_SKILLS) {
            bool actors_out_of_rotation = true;
            bool no_active_skills_for_actors = true;
            for (auto entity : registry.view<component::is_actor>()) {
                if (!termination_condition.actor.empty() &&
                    utils::get_entity_name(entity, registry) != termination_condition.actor) {
                    continue;
                }
                if (!registry.any_of<component::rotation_component>(entity)) {
                    continue;
                }
                if (!registry.any_of<component::no_more_rotation>(entity)) {
                    actors_out_of_rotation = false;
                    break;
                }
                if (registry.any_of<component::skills_ticks_tracker_component,
                                    component::skills_actions_component,
                                    component::finished_skills_actions_component,
                                    component::destroy_skills_ticks_tracker_component>(entity)) {
                    no_active_skills_for_actors = false;
                    break;
                }
            }
            if (actors_out_of_rotation && no_active_skills_for_actors) {
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
                if (utils::get_entity_name(actor_entity, registry) != actor.name) {
                    continue;
                }
                auto& existing_rotation_component =
                    registry.get<component::rotation_component>(actor_entity);
                auto existing_rotation_size =
                    existing_rotation_component.rotation.skill_casts.size();
                if (existing_rotation_size == actor.rotation.skill_casts.size()) {
                    break;
                }
                registry.remove<component::no_more_rotation>(actor_entity);
                std::transform(
                    encounter.actors[0].rotation.skill_casts.begin() + existing_rotation_size,
                    encounter.actors[0].rotation.skill_casts.end(),
                    std::back_inserter(existing_rotation_component.rotation.skill_casts),
                    [](const configuration::skill_cast_t& skill_cast) {
                        return actor::skill_cast_t{skill_cast.skill, skill_cast.cast_time_ms};
                    });
                break;
            }
        }
    } else {
        registry.ctx().emplace<tick_t>(0);
        system::setup_encounter(registry, encounter);
    }

    std::string result;
    try {
        system::setup_combat_stats(registry);
        while (continue_combat_loop(registry, encounter)) {
            registry.ctx().get<tick_t>() += 1;
            tick(registry);
        }
        result = utils::to_string(system::get_audit_report(registry, encounter.audit_offset));
    } catch (std::exception& e) {
        spdlog::error("Exception: {}", e.what());
        result =
            utils::to_string(system::get_audit_report(registry, encounter.audit_offset, e.what()));
    }

    spdlog::info("[{}] combat loop completed for encounter.", utils::get_current_tick(registry));

    auto cache_key = convert_encounter_to_cache_key(encounter);
    if (!registry_cache.contains(cache_key)) {
        registry_cache.put(cache_key, std::move(registry));
    }
    return result;
}

}  // namespace gw2combat

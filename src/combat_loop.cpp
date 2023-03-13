#include "combat_loop.hpp"

#include "component/actor/combat_stats.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/equipment/bundle.hpp"
#include "component/lifecycle/destroy_entity.hpp"
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

#include "utils/entity_utils.hpp"

namespace gw2combat {

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::actor_created,
                   component::equipped_bundle,
                   component::dropped_bundle,
                   component::relative_attributes,
                   component::incoming_damage,
                   component::animation_expired,
                   component::cooldown_expired,
                   component::duration_expired,
                   component::outgoing_strikes_component,
                   component::outgoing_effects_component,
                   component::incoming_strikes_component,
                   component::incoming_effects_component,
                   component::incoming_damage,
                   component::combat_stats_updated>();
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
    system::progress_cooldowns(registry);
    system::progress_durations(registry);

    system::perform_skills(registry);
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

    system::reset_counters(registry);
    system::cleanup_finished_casting_skills(registry);
    destroy_marked_entities(registry);
    clear_temporary_components(registry);
}

std::string combat_loop(const configuration::encounter_t& encounter) {
    registry_t registry;
    system::setup_encounter(registry, encounter);

    try {
        tick_t current_tick = 1;
        registry.ctx().emplace<const tick_t&>(current_tick);
        bool continue_combat_loop = true;
        while (continue_combat_loop) {
            tick(registry);

            for (auto entity : registry.view<component::is_actor>()) {
                if (registry.any_of<component::is_downstate>(entity)) {
                    spdlog::info("[{}] {} is downstate",
                                 current_tick,
                                 utils::get_entity_name(entity, registry));
                    continue_combat_loop = false;
                    break;
                }
            }

            for (auto& termination_condition : encounter.termination_conditions) {
                if (termination_condition.type ==
                    configuration::termination_condition_t::type_t::ROTATION) {
                    bool everyone_out_of_rotation = true;
                    for (auto entity : registry.view<component::is_actor>()) {
                        if (utils::get_entity_name(entity, registry) !=
                                termination_condition.actor ||
                            !registry.any_of<component::rotation_component>(entity)) {
                            continue;
                        }
                        if (!registry.any_of<component::no_more_rotation>(entity)) {
                            everyone_out_of_rotation = false;
                            break;
                        }
                    }
                    if (everyone_out_of_rotation) {
                        continue_combat_loop = false;
                        break;
                    }
                } else if (termination_condition.type ==
                           configuration::termination_condition_t::type_t::DAMAGE) {
                    bool someone_took_required_damage = false;
                    for (auto entity : registry.view<component::is_actor>()) {
                        if (utils::get_entity_name(entity, registry) !=
                            termination_condition.actor) {
                            continue;
                        }
                        int max_health = utils::round_to_nearest_even(
                            registry.get<component::static_attributes>(entity)
                                .attribute_value_map[actor::attribute_t::MAX_HEALTH]);
                        int current_health = registry.get<component::combat_stats>(entity).health;
                        if (max_health - current_health >= termination_condition.damage) {
                            someone_took_required_damage = true;
                            break;
                        }
                    }
                    if (someone_took_required_damage) {
                        continue_combat_loop = false;
                        break;
                    }
                } else if (termination_condition.type ==
                           configuration::termination_condition_t::type_t::TIME) {
                    if (current_tick >= termination_condition.time) {
                        continue_combat_loop = false;
                        break;
                    }
                }
            }

            ++current_tick;
        }
    } catch (std::exception& e) {
        spdlog::error("Exception: {}", e.what());
    }

    return nlohmann::json{system::get_audit_report(registry)}[0].dump();
}

}  // namespace gw2combat

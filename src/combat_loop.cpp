#include "combat_loop.hpp"

#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/damage/strikes_pipeline.hpp"
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
#include "utils/io_utils.hpp"

namespace gw2combat {

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

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::relative_attributes,
                   component::incoming_damage,
                   component::animation_expired,
                   component::cooldown_expired,
                   component::duration_expired,
                   component::outgoing_strikes_component,
                   component::outgoing_effects_component,
                   component::incoming_strikes_component,
                   component::incoming_effects_component,
                   component::incoming_damage>();
    if (registry.any_of<component::entity_was_destroyed>(utils::get_singleton_entity())) {
        clean_children_of_destroyed_owners(registry);
        registry.remove<component::entity_was_destroyed>(utils::get_singleton_entity());
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

    system::audit(registry);

    system::reset_counters(registry);
    system::cleanup_expired_components(registry);
    system::cleanup_finished_casting_skills(registry);
    system::destroy_actors_with_no_rotation(registry);

    clear_temporary_components(registry);
}

std::string combat_loop(const configuration::encounter_t& encounter) {
    registry_t registry;
    system::setup_server_encounter(registry, encounter);

    tick_t current_tick = 1;
    registry.ctx().emplace<const tick_t&>(current_tick);
    tick_t everyone_out_of_rotation_at_tick = 3'000'000'000;
    while (true) {
        tick(registry);

        bool everyone_out_of_rotation = true;
        auto actors = registry.view<component::is_actor>();
        for (auto entity : actors) {
            if (registry.any_of<component::is_downstate>(entity)) {
                spdlog::info(
                    "[{}] {} is downstate", current_tick, utils::get_entity_name(entity, registry));
                goto end_of_combat_loop;
            }
            if (!registry.any_of<component::rotation_component>(entity)) {
                continue;
            }
            if (!registry.any_of<component::no_more_rotation>(entity)) {
                everyone_out_of_rotation = false;
            }
        }
        if (everyone_out_of_rotation) {
            if (everyone_out_of_rotation_at_tick > current_tick) {
                everyone_out_of_rotation_at_tick = current_tick;
            }
            if (current_tick - everyone_out_of_rotation_at_tick >= 10'000) {
                spdlog::info("[{}] no one has any rotation left!", current_tick);
                break;
            }
        }

        ++current_tick;
    }
end_of_combat_loop:
    return nlohmann::json{system::get_audit_report(registry)}[0].dump();
}

}  // namespace gw2combat

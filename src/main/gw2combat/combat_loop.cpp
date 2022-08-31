#include "combat_loop.hpp"

#include "common.hpp"

#include "init.hpp"

#include "utilities/logging_utilities.hpp"

#include "system/system.hpp"

#include "gw2combat/component/actor/destroy_after_rotation.hpp"
#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/is_actor.hpp"
#include "gw2combat/component/actor/is_downstate.hpp"
#include "gw2combat/component/actor/no_more_rotation.hpp"
#include "gw2combat/component/damage/effects_pipeline.hpp"
#include "gw2combat/component/damage/incoming_damage.hpp"
#include "gw2combat/component/damage/incoming_damage_modifiers.hpp"
#include "gw2combat/component/damage/outgoing_damage_modifiers.hpp"
#include "gw2combat/component/damage/strikes_pipeline.hpp"
#include "gw2combat/component/effect/duration.hpp"
#include "gw2combat/component/skill/finished_animation.hpp"

namespace gw2combat {

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::effective_attributes,
                   component::outgoing_damage_modifiers,
                   component::incoming_damage_modifiers,
                   component::outgoing_strikes_component,
                   component::incoming_strikes_component,
                   component::outgoing_effects_component,
                   component::incoming_effects_component,
                   component::incoming_damage,
                   component::finished_animation>();
}

void do_tick(registry_t& registry) {
    system::init_combat_stats(registry);

    system::progress_recharges(registry);
    system::progress_cooldowns(registry);
    system::progress_durations(registry);

    system::expire_non_damaging_effects_with_no_duration(registry);

    system::calculate_effective_attributes(registry);
    // utils::log_component<component::effective_attributes>(registry);

    system::continue_rotation(registry, true);
    // utils::log_component<component::animation>(registry);

    system::check_if_animation_finished(registry);
    // utils::log_component<component::animation>(registry);
    // utils::log_component<component::finished_animation>(registry);

    system::do_skill(registry);
    // utils::log_component<component::outgoing_strikes_component>(registry);

    system::dispatch_strikes(registry);

    system::apply_incoming_strikes(registry);
    // utils::log_component<component::incoming_strikes_component>(registry);
    // utils::log_component<component::outgoing_effects_component>(registry);

    system::do_skill_triggers(registry);

    system::dispatch_effects(registry);
    // utils::log_component<component::incoming_effects_component>(registry);

    system::apply_incoming_effects(registry);
    // utils::log_component<component::effects_component>(registry);
    // utils::log_component<component::duration>(registry);
    // registry.view<component::effects_component>().each(
    //    [&](entity_t entity, const component::effects_component& effects_component) {
    //        auto effect_counts = effects_component.count_all();
    //        spdlog::info("[{}] {} - {}",
    //                     utils::get_current_tick(registry),
    //                     utils::get_entity_name(entity, registry),
    //                     utils::to_string(effect_counts));
    //    });

    system::expire_damaging_effects_with_no_duration(registry);

    if (tick_t current_tick = utils::get_current_tick(registry);
        (current_tick != 0) && (current_tick % 1'000 == 0)) {
        system::buffer_condition_damage(registry);
        system::apply_condition_damage(registry);
    }

    system::progress_animations(registry);
    system::remove_animation_if_finished(registry);
    // utils::log_component<component::finished_animation>(registry);
    // registry.view<component::no_more_rotation, component::destroy_after_rotation>().each(
    //    [&](entity_t entity) {
    //        spdlog::info("entity: {} destroyed after rota",
    //                     utils::get_entity_name(entity, registry));
    //    });
    system::destroy_after_rotation(registry);

    system::update_combat_stats(registry);

    clear_temporary_components(registry);
}

void combat_loop() {
    registry_t registry;
    build_actors(registry);
    build_skills(registry);

    tick_t current_tick{0};
    registry.ctx().emplace<const tick_t&>(current_tick);

    tick_t everyone_out_of_rotation_at_tick = 3'000'000'000;
    while (true) {
        do_tick(registry);

        bool everyone_out_of_rotation = true;
        auto actors = registry.view<component::is_actor>();
        for (auto entity : actors) {
            if (registry.ctx().at<std::string>(entity).ends_with("technician")) {
                continue;
            }
            if (registry.any_of<component::is_downstate>(entity)) {
                spdlog::info(
                    "[{}] {} is downstate", current_tick, utils::get_entity_name(entity, registry));
                return;
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
                break;
            }
        }

        ++current_tick;
    }
}

}  // namespace gw2combat

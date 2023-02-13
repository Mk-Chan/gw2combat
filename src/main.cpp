#include "common.hpp"

#include "component/actor/did_weapon_swap.hpp"
#include "component/actor/effects_component.hpp"
#include "component/damage/incoming_damage.hpp"

#include "system/actor.hpp"
#include "system/effects.hpp"
#include "system/encounter.hpp"
#include "system/temporal.hpp"

#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/temporal/animation_component.hpp"
#include "component/temporal/cooldown_component.hpp"
#include "component/temporal/duration_component.hpp"

#include "utils/entity_utils.hpp"

namespace gw2combat {

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::relative_attributes,
                   component::incoming_damage,
                   component::did_weapon_swap,
                   component::animation_expired,
                   component::cooldown_expired,
                   component::duration_expired>();
}

void tick(registry_t& registry) {
    system::setup_combat_stats(registry);

    system::perform_rotation(registry);

    system::progress_animations(registry);
    system::progress_cooldowns(registry);
    system::progress_durations(registry);

    system::calculate_relative_attributes(registry);

    system::buffer_damage_for_effects_with_no_duration(registry);
    if (tick_t current_tick = utils::get_current_tick(registry); current_tick % 1000 == 0) {
        system::buffer_condition_damage(registry);
        system::apply_condition_damage(registry);
    }

    system::update_combat_stats(registry);

    system::cleanup_expired_components(registry);
    system::cleanup_expired_effects(registry);

    clear_temporary_components(registry);
}

void combat_loop() {
    registry_t registry;

    system::setup_encounter(registry);

    tick_t current_tick = 1;
    registry.ctx().emplace<const tick_t&>(current_tick);

    tick_t everyone_out_of_rotation_at_tick = 3'000'000'000;
    while (true) {
        tick(registry);

        bool everyone_out_of_rotation = true;
        auto actors = registry.view<component::is_actor>();
        for (auto entity : actors) {
            if (registry.ctx().get<std::string>(entity).ends_with("technician") ||
                !registry.any_of<component::rotation_component>(entity)) {
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
                spdlog::info("No one has any rotation left!");
                break;
            }
        }

        ++current_tick;
    }
}

}  // namespace gw2combat

int main() {
    gw2combat::combat_loop();
    return 0;
}

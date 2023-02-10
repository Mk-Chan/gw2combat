#include "common.hpp"

#include "component/actor/effects_component.hpp"
#include "component/actor/did_weapon_swap.hpp"
#include "component/damage/buffered_condition_damage.hpp"
#include "component/damage/incoming_damage.hpp"

#include "system/actor.hpp"
#include "system/effects.hpp"
#include "system/encounter.hpp"
#include "system/temporal.hpp"

#include "component/temporal/animation_component.hpp"
#include "component/temporal/cooldown_component.hpp"
#include "component/temporal/duration_component.hpp"
#include "utils/io_utils.hpp"

namespace gw2combat {

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::effective_attributes,
                   component::incoming_damage,
                   component::did_weapon_swap,
                   component::animation_expired,
                   component::cooldown_expired,
                   component::duration_expired>();
}

void tick(registry_t& registry) {
    system::setup_combat_stats(registry);

    system::progress_animations(registry);
    system::progress_cooldowns(registry);
    system::progress_durations(registry);

    system::calculate_effective_attributes(registry);

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

    while (true) {
        tick(registry);

        ++current_tick;
        if (current_tick == 2002) {
            break;
        }
    }
}

}  // namespace gw2combat

int main() {
    gw2combat::combat_loop();
    return 0;
}

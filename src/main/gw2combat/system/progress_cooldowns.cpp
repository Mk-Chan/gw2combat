#include "system.hpp"

#include "gw2combat/utilities/base_utilities.hpp"
#include "gw2combat/utilities/entity_utilities.hpp"

#include "gw2combat/component/actor/alacrity.hpp"
#include "gw2combat/component/cooldown.hpp"
#include "gw2combat/component/skill/ammo.hpp"

namespace gw2combat::system {

void progress_cooldowns(registry_t& registry) {
    registry.view<component::cooldown>().each([&](entity_t entity, component::cooldown& cooldown) {
        if (cooldown.duration[0] == 0) {
            if (registry.any_of<component::ammo>(entity)) {
                ++registry.get<component::ammo>(entity).current_ammo;
            }
            registry.remove<component::cooldown>(entity);
            return;
        }

        bool has_alacrity =
            registry.any_of<component::alacrity>(utils::get_owner(entity, registry));
        cooldown.progress[0] += !has_alacrity;
        cooldown.progress[1] += has_alacrity;

        int no_alacrity_progress_pct = cooldown.progress[0] * 100 / cooldown.duration[0];
        int alacrity_progress_pct = cooldown.progress[1] * 100 / cooldown.duration[1];

        if (no_alacrity_progress_pct + alacrity_progress_pct >= 100) {
            if (registry.any_of<component::ammo>(entity)) {
                auto& ammo = registry.get<component::ammo>(entity);
                ++ammo.current_ammo;

                if (ammo.current_ammo == ammo.max_ammo) {
                    registry.remove<component::cooldown>(entity);
                }
            }
            return;
        }
    });
}

}  // namespace gw2combat::system

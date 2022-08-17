#include "system.hpp"

#include "gw2combat/utilities/base_utilities.hpp"
#include "gw2combat/utilities/entity_utilities.hpp"

#include "gw2combat/component/actor/alacrity.hpp"
#include "gw2combat/component/cooldown.hpp"

namespace gw2combat::system {

void progress_cooldowns(registry_t& registry) {
    registry.view<component::cooldown>().each([&](entity_t entity, component::cooldown& cooldown) {
        if (cooldown.duration[0] == 0) {
            registry.remove<component::cooldown>(entity);
            return;
        }

        int no_alacrity_progress_pct = cooldown.progress[0] * 100 / cooldown.duration[0];
        int alacrity_progress_pct = cooldown.progress[1] * 100 / cooldown.duration[1];

        if (no_alacrity_progress_pct + alacrity_progress_pct >= 100) {
            registry.remove<component::cooldown>(entity);
            spdlog::info("{}: {} off cd", utils::get_current_tick(registry), utils::get_entity_name(entity, registry));
            return;
        }

        bool has_alacrity = registry.any_of<component::alacrity>(entity);
        cooldown.progress[0] += !has_alacrity;
        cooldown.progress[1] += has_alacrity;
    });
}

}  // namespace gw2combat::system

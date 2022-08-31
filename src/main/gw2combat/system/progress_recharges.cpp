#include "system.hpp"

#include "gw2combat/component/skill/recharge.hpp"

namespace gw2combat::system {

void progress_recharges(registry_t& registry) {
    registry.view<component::recharge>().each([&](entity_t entity, component::recharge& recharge) {
        ++recharge.progress;
        if (recharge.progress >= recharge.duration) {
            registry.remove<component::recharge>(entity);
            return;
        }
    });
}

}  // namespace gw2combat::system

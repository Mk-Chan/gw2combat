#include "system.hpp"

#include "gw2combat/component/actor/destroy_after_rotation.hpp"
#include "gw2combat/component/actor/no_more_rotation.hpp"

namespace gw2combat::system {

void destroy_after_rotation(registry_t& registry) {
    registry.view<component::no_more_rotation, component::destroy_after_rotation>().each(
        [&](entity_t entity) { registry.destroy(entity); });
}

}  // namespace gw2combat::system

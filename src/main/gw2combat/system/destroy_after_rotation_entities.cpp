#include "system.hpp"

#include "gw2combat/component/character/destroy_after_rotation.hpp"
#include "gw2combat/component/character/no_more_rotation.hpp"
#include "gw2combat/component/skills/normal_cast_skill.hpp"

namespace gw2combat::system {

void destroy_after_rotation_entities(registry_t& registry, tick_t current_tick) {
    registry
        .view<component::destroy_after_rotation, component::no_more_rotation>(
            entt::exclude<component::normal_cast_skill>)
        .each([&](entity_t entity) { registry.destroy(entity); });
}

}  // namespace gw2combat::system

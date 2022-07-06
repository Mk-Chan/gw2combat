#ifndef GW2COMBAT_ENTITY_HPP
#define GW2COMBAT_ENTITY_HPP

#include <entt/entt.hpp>

namespace gw2combat {

extern std::unique_ptr<entt::entity> singleton_entity;

extern void init_entities(entt::registry& registry);

}  // namespace gw2combat

#endif  // GW2COMBAT_ENTITY_HPP

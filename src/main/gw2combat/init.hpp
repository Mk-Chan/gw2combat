#ifndef GW2COMBAT_INIT_HPP
#define GW2COMBAT_INIT_HPP

#include "common.hpp"

namespace gw2combat {

entity_t build_actor(registry_t& registry, const std::string& name, int team_id);
void build_actors(registry_t& registry);
void build_skills(registry_t& registry);

}  // namespace gw2combat

#endif  // GW2COMBAT_INIT_HPP

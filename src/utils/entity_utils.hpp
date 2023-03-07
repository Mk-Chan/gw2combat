#ifndef GW2COMBAT_UTILITIES_ENTITY_UTILS_HPP
#define GW2COMBAT_UTILITIES_ENTITY_UTILS_HPP

#include "basic_utils.hpp"

#include "component/hierarchy/owner_component.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline std::string get_entity_name(entity_t entity, registry_t& registry) {
    if (!registry.ctx().contains<std::string>(entity)) {
        return "temporary_entity";
    }
    return registry.ctx().get<std::string>(entity);
}

[[nodiscard]] static inline entity_t get_owner(entity_t entity, registry_t& registry) {
    entity_t current_entity = entity;
    while (registry.any_of<component::owner_component>(current_entity)) {
        current_entity = registry.get<component::owner_component>(current_entity).entity;
    }
    return current_entity;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_ENTITY_UTILS_HPP

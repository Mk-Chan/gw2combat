#ifndef GW2COMBAT_UTILITIES_BASE_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_BASE_UTILITIES_HPP

#include "gw2combat/component/damage/source_entity.hpp"
#include "gw2combat/types.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline std::string get_entity_name(entity_t entity, registry_t& registry) {
    if (!registry.ctx().contains<entt::hashed_string>(to_u32(entity))) {
        return "temporary_entity";
    }
    return std::string{registry.ctx().at<entt::hashed_string>(to_u32(entity)).data()};
}

[[nodiscard]] static inline entity_t get_source_entity(entity_t entity, registry_t& registry) {
    entity_t current_entity = entity;
    while (true) {
        auto source_entity_ptr = registry.try_get<component::source_entity>(current_entity);
        if (source_entity_ptr == nullptr) {
            break;
        }

        current_entity = source_entity_ptr->entity;
    }
    return current_entity;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_BASE_UTILITIES_HPP

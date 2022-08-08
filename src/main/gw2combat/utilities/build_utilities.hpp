#ifndef GW2COMBAT_UTILITIES_BUILD_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_BUILD_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/build.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline bool has_trait(trait_type trait,
                                           const component::traits_component& traits_component) {
    return ranges::find(traits_component.traits, trait) != ranges::end(traits_component.traits);
}

[[nodiscard]] static inline bool has_trait(trait_type trait,
                                           const component::traits_component* traits_component) {
    if (traits_component == nullptr) {
        return false;
    }
    return has_trait(trait, *traits_component);
}

[[nodiscard]] static inline bool has_trait(trait_type trait,
                                           entity_t entity,
                                           registry_t& registry) {
    auto traits_component_ptr = registry.try_get<component::traits_component>(entity);
    return has_trait(trait, traits_component_ptr);
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_BUILD_UTILITIES_HPP

#ifndef GW2COMBAT_UTILITIES_BASE_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_BASE_UTILITIES_HPP

#include <random>

#include "gw2combat/component/damage/source_entity.hpp"
#include "gw2combat/types.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline double get_random_0_100() {
    static std::random_device random_device;
    static unsigned int rng_seed = random_device();
    // static unsigned int rng_seed = 297364872;
    static std::mt19937 generator(rng_seed);
    static std::uniform_real_distribution distribution(0.0, 100.0);
    return distribution(generator);
}

[[nodiscard]] static inline bool check_random_success(double upper_bound) {
    return get_random_0_100() < upper_bound;
}

[[nodiscard]] static inline std::string get_entity_name(entity_t entity, registry_t& registry) {
    if (!registry.ctx().contains<std::string>(to_u32(entity))) {
        return "temporary_entity";
    }
    return registry.ctx().at<std::string>(to_u32(entity));
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

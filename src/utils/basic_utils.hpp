#ifndef GW2COMBAT_UTILS_BASIC_UTILS_HPP
#define GW2COMBAT_UTILS_BASIC_UTILS_HPP

#include <random>

#include "common.hpp"

#include "component/encounter/random_state.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline int round_down(double value) {
    return static_cast<int>(std::floor(value));
}

[[nodiscard]] static inline int round_to_nearest_even(double value) {
    return static_cast<int>(std::nearbyint(value));
}

[[nodiscard]] static inline double round_to_nearest_n_digits(double value, int n) {
    return utils::round_to_nearest_even(value * std::pow(10.0, n)) / std::pow(10.0, n);
}

template <typename T>
[[nodiscard]] static inline std::string to_string(T t) {
    return nlohmann::json{t}[0].dump();
}

[[nodiscard]] static inline int get_random(int min_inclusive, int max_inclusive,
                                         registry_t& registry) {
    std::uniform_int_distribution distribution(min_inclusive, max_inclusive);
    return distribution(registry.ctx().get<component::random_state_t>().generator);
}

[[nodiscard]] static inline double get_random(double min_inclusive, double max_inclusive,
                                            registry_t& registry) {
    std::uniform_real_distribution distribution(min_inclusive, max_inclusive);
    return distribution(registry.ctx().get<component::random_state_t>().generator);
}

[[nodiscard]] static inline double get_random_0_100_int(registry_t& registry) {
    return get_random(0, 99, registry);
}

[[nodiscard]] static inline double get_random_0_100(registry_t& registry) {
    return get_random(0.0, 100.0, registry);
}

[[nodiscard]] static inline bool check_random_success(int upper_bound, registry_t& registry) {
    return get_random_0_100_int(registry) < upper_bound;
}

[[nodiscard]] static inline bool check_random_success(double upper_bound, registry_t& registry) {
    return get_random_0_100(registry) < upper_bound;
}

[[nodiscard]] static inline tick_t get_current_tick(registry_t& registry) {
    return registry.ctx().get<const tick_t>();
}

[[nodiscard]] static inline entity_t get_singleton_entity() {
    return entity_t{0};
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_BASIC_UTILS_HPP

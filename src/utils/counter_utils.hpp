#ifndef GW2COMBAT_UTILS_COUNTER_UTILS_HPP
#define GW2COMBAT_UTILS_COUNTER_UTILS_HPP

#include "common.hpp"

#include "configuration/counter_configuration.hpp"

#include "component/counter/is_counter.hpp"

namespace gw2combat::utils {

static inline void apply_counter_modifications(
    registry_t& registry,
    component::is_counter& is_counter,
    const configuration::counter_modifier_t& counter_modifier) {
    auto operation_fn = [&](int modifier) {
        switch (counter_modifier.operation) {
            case configuration::counter_modifier_t::operation_t::ADD:
                is_counter.value += modifier;
                break;
            case configuration::counter_modifier_t::operation_t::SUBTRACT:
                is_counter.value -= modifier;
                break;
            case configuration::counter_modifier_t::operation_t::SET:
                is_counter.value = modifier;
                break;
            default:
                break;
        }
    };
    if (counter_modifier.operation == configuration::counter_modifier_t::operation_t::RESET) {
        is_counter.value = is_counter.counter_configuration.initial_value;
    } else {
        if (counter_modifier.counter_value) {
            registry.view<component::is_counter>().each(
                [&](component::is_counter& referenced_counter) {
                    if (*counter_modifier.counter_value ==
                        referenced_counter.counter_configuration.counter_key) {
                        operation_fn(referenced_counter.value);
                    }
                });
        }
        if (counter_modifier.value) {
            operation_fn(*counter_modifier.value);
        }
    }
}

static inline component::is_counter& get_counter(const actor::counter_t& counter_key,
                                                 registry_t& registry) {
    auto view = registry.view<component::is_counter>();
    for (auto&& [entity, is_counter] : view.each()) {
        if (is_counter.counter_configuration.counter_key == counter_key) {
            return is_counter;
        }
    }
    throw std::runtime_error(fmt::format("Counter with key {} not found", counter_key));
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_COUNTER_UTILS_HPP

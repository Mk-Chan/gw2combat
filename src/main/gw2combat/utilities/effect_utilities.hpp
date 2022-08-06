#ifndef GW2COMBAT_UTILITIES_EFFECT_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_EFFECT_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/component/effects_component.hpp"
#include "gw2combat/effects.hpp"

namespace gw2combat::utils {

constexpr static inline std::array BOONS{
    effects::effect_type::AEGIS,
    effects::effect_type::ALACRITY,
    effects::effect_type::FURY,
    effects::effect_type::MIGHT,
    effects::effect_type::QUICKNESS,
    effects::effect_type::RESOLUTION,
};

[[nodiscard]] static inline effects::effect_stacking_type get_effect_stacking_type(
    effects::effect_type effect_type) {
    switch (effect_type) {
        case effects::effect_type::AEGIS:
        case effects::effect_type::ALACRITY:
        case effects::effect_type::FURY:
        case effects::effect_type::QUICKNESS:
        case effects::effect_type::RESOLUTION:
        case effects::effect_type::BINDING_BLADE:
        case effects::effect_type::VIRTUE_OF_JUSTICE:
        case effects::effect_type::INSPIRING_VIRTUE:
            return effects::effect_stacking_type::STACKING_DURATION;
        case effects::effect_type::MIGHT:
        case effects::effect_type::VULNERABILITY:
        case effects::effect_type::BURNING:
        case effects::effect_type::BLEEDING:
        case effects::effect_type::SYMBOLIC_AVENGER:
            return effects::effect_stacking_type::STACKING_INTENSITY;
    }
}

[[nodiscard]] static inline size_t get_max_stored_stacks_of_effect_type(
    effects::effect_type effect_type) {
    switch (effect_type) {
        case effects::effect_type::AEGIS:
        case effects::effect_type::ALACRITY:
        case effects::effect_type::FURY:
        case effects::effect_type::QUICKNESS:
        case effects::effect_type::RESOLUTION:
        case effects::effect_type::BINDING_BLADE:
        case effects::effect_type::VIRTUE_OF_JUSTICE:
        case effects::effect_type::INSPIRING_VIRTUE:
            return 1;
        case effects::effect_type::MIGHT:
        case effects::effect_type::VULNERABILITY:
        case effects::effect_type::BURNING:
        case effects::effect_type::BLEEDING:
        case effects::effect_type::SYMBOLIC_AVENGER:
            return 1500;
    }
}

[[nodiscard]] static inline size_t get_max_considered_stacks_of_effect_type(
    effects::effect_type effect_type) {
    switch (effect_type) {
        case effects::effect_type::AEGIS:
        case effects::effect_type::ALACRITY:
        case effects::effect_type::FURY:
        case effects::effect_type::QUICKNESS:
        case effects::effect_type::RESOLUTION:
        case effects::effect_type::BINDING_BLADE:
        case effects::effect_type::VIRTUE_OF_JUSTICE:
        case effects::effect_type::INSPIRING_VIRTUE:
            return 1;
        case effects::effect_type::SYMBOLIC_AVENGER:
            return 5;
        case effects::effect_type::MIGHT:
        case effects::effect_type::VULNERABILITY:
            return 25;
        case effects::effect_type::BURNING:
        case effects::effect_type::BLEEDING:
            return 1500;
    }
}

[[nodiscard]] static inline size_t get_max_effect_duration(effects::effect_type effect_type) {
    switch (effect_type) {
        case effects::effect_type::AEGIS:
        case effects::effect_type::ALACRITY:
        case effects::effect_type::FURY:
        case effects::effect_type::MIGHT:
        case effects::effect_type::QUICKNESS:
        case effects::effect_type::RESOLUTION:
            // FIXME: Commented for testing only
            // return 30;
        case effects::effect_type::BINDING_BLADE:
        case effects::effect_type::VIRTUE_OF_JUSTICE:
        case effects::effect_type::INSPIRING_VIRTUE:
        case effects::effect_type::SYMBOLIC_AVENGER:
        case effects::effect_type::VULNERABILITY:
        case effects::effect_type::BURNING:
        case effects::effect_type::BLEEDING:
            return 1'000'000'000;
    }
}

[[nodiscard]] static inline tick_t get_effective_effect_duration(
    tick_t duration,
    effects::effect_type effect_type,
    const component::effective_attributes& effective_attributes) {
    auto calculate_condition_duration = [&](double special_condition_duration_pct = 0.0) {
        return tick_t{(unsigned int)((double)duration *
                                     (1.0 + std::max(effective_attributes.condition_duration_pct,
                                                     special_condition_duration_pct) /
                                                100.0))};
    };
    auto calculate_boon_duration = [&](double special_boon_duration_pct = 0.0) {
        return tick_t{(unsigned int)((double)duration *
                                     (1.0 + std::max(effective_attributes.boon_duration_pct,
                                                     special_boon_duration_pct) /
                                                100.0))};
    };

    switch (effect_type) {
        case effects::effect_type::AEGIS:
        case effects::effect_type::ALACRITY:
        case effects::effect_type::FURY:
        case effects::effect_type::MIGHT:
        case effects::effect_type::QUICKNESS:
        case effects::effect_type::RESOLUTION:
            return calculate_boon_duration();

        case effects::effect_type::VULNERABILITY:
            return calculate_condition_duration();

        case effects::effect_type::BURNING:
            return calculate_condition_duration(effective_attributes.burning_duration_pct);
        case effects::effect_type::BLEEDING:
            return calculate_condition_duration(effective_attributes.bleeding_duration_pct);

        case effects::effect_type::BINDING_BLADE:
        case effects::effect_type::VIRTUE_OF_JUSTICE:
        case effects::effect_type::SYMBOLIC_AVENGER:
        case effects::effect_type::INSPIRING_VIRTUE:
            return tick_t{duration};
    }
}

[[nodiscard]] constexpr static inline bool is_boon(effects::effect_type effect_type) {
    switch (effect_type) {
        case effects::effect_type::AEGIS:
        case effects::effect_type::ALACRITY:
        case effects::effect_type::FURY:
        case effects::effect_type::MIGHT:
        case effects::effect_type::QUICKNESS:
        case effects::effect_type::RESOLUTION:
            return true;

        case effects::effect_type::BURNING:
        case effects::effect_type::BLEEDING:
        case effects::effect_type::BINDING_BLADE:
        case effects::effect_type::VULNERABILITY:
        case effects::effect_type::VIRTUE_OF_JUSTICE:
        case effects::effect_type::SYMBOLIC_AVENGER:
        case effects::effect_type::INSPIRING_VIRTUE:
            return false;
    }
}

[[nodiscard]] constexpr static inline bool is_damaging_condition(effects::effect_type effect_type) {
    switch (effect_type) {
        case effects::effect_type::BURNING:
        case effects::effect_type::BLEEDING:
        case effects::effect_type::BINDING_BLADE:
            return true;

        case effects::effect_type::AEGIS:
        case effects::effect_type::ALACRITY:
        case effects::effect_type::FURY:
        case effects::effect_type::MIGHT:
        case effects::effect_type::QUICKNESS:
        case effects::effect_type::RESOLUTION:
        case effects::effect_type::VULNERABILITY:
        case effects::effect_type::VIRTUE_OF_JUSTICE:
        case effects::effect_type::SYMBOLIC_AVENGER:
        case effects::effect_type::INSPIRING_VIRTUE:
            return false;
    }
}

[[nodiscard]] static inline double calculate_condition_damage(effects::effect_type effect_type,
                                                              effects::effect_stack effect_stack,
                                                              registry_t& registry) {
    if (!is_damaging_condition(effect_type)) {
        return 0.0;
    }

    auto source_entity = get_source_entity(effect_stack.source, registry);
    auto&& [source_effective_attributes, outgoing_condition_damage_multiplier] =
        registry
            .get<component::effective_attributes, component::outgoing_condition_damage_multiplier>(
                source_entity);
    switch (effect_type) {
        case effects::effect_type::BURNING:
            return (131.0 + 0.155 * source_effective_attributes.condition_damage) *
                   outgoing_condition_damage_multiplier.multiplier * (double)effect_stack.progress /
                   (double)effects::effect_pulse_rate;
        case effects::effect_type::BLEEDING:
            return (22.0 + 0.06 * source_effective_attributes.condition_damage) *
                   outgoing_condition_damage_multiplier.multiplier * (double)effect_stack.progress /
                   (double)effects::effect_pulse_rate;
        case effects::effect_type::BINDING_BLADE:
            return (160.0 + 0.2 * source_effective_attributes.power) *
                   (double)effect_stack.progress / (double)effects::effect_pulse_rate;
        default:
            spdlog::error("missing damage formula for effect_type: {}",
                          nlohmann::json{effect_type}[0].dump());
            throw std::exception();
    }
}

[[nodiscard]] static inline double calculate_condition_damage(auto effect_type,
                                                              auto effect_stacks,
                                                              registry_t& registry) {
    for (const auto& effect_stack : effect_stacks) {
        return calculate_condition_damage(effect_type, effect_stack, registry);
    }
}

[[nodiscard]] static inline size_t get_num_stacks_of_effect(
    effects::effect_type effect_type,
    const component::effects_component& effects_component) {
    return std::min(effects_component.effects.count(effect_type),
                    get_max_considered_stacks_of_effect_type(effect_type));
}

[[nodiscard]] static inline size_t get_num_stacks_of_effect(
    effects::effect_type effect_type,
    const component::effects_component* effects_component) {
    if (effects_component == nullptr) {
        return 0;
    }
    return get_num_stacks_of_effect(effect_type, *effects_component);
}

[[nodiscard]] static inline size_t get_num_stacks_of_effect(effects::effect_type effect_type,
                                                            entity_t entity,
                                                            registry_t& registry) {
    auto effects_component_ptr = registry.try_get<component::effects_component>(entity);
    return get_num_stacks_of_effect(effect_type, effects_component_ptr);
}

[[nodiscard]] static inline bool has_effect(effects::effect_type effect_type,
                                            const component::effects_component& effects_component) {
    return effects_component.effects.contains(effect_type);
}

[[nodiscard]] static inline bool has_effect(effects::effect_type effect_type,
                                            const component::effects_component* effects_component) {
    if (effects_component == nullptr) {
        return false;
    }
    return has_effect(effect_type, *effects_component);
}

[[nodiscard]] static inline bool has_effect(effects::effect_type effect_type,
                                            entity_t entity,
                                            registry_t& registry) {
    auto effects_component_ptr = registry.try_get<component::effects_component>(entity);
    return has_effect(effect_type, effects_component_ptr);
}

[[nodiscard]] static inline bool is_effect_affected_by_incoming_multiplier(
    effects::effect_type effect_type) {
    return effect_type != effects::effect_type::BINDING_BLADE;
}

static inline void apply_effects(const effects::effect_application& effect_application,
                                 component::effects_component& effects_component) {
    effects::effect_type effect_type = effect_application.effect_type;
    auto& effects = effects_component.effects;
    if (get_effect_stacking_type(effect_type) ==
        effects::effect_stacking_type::STACKING_INTENSITY) {
        for (size_t i = 0; i < effect_application.num_stacks; ++i) {
            if (get_num_stacks_of_effect(effect_type, effects_component) <
                get_max_stored_stacks_of_effect_type(effect_type)) {
                effects.emplace(
                    effect_type,
                    effects::effect_stack{effect_application.source, effect_application.duration});
            }
        }
    } else if (!effects.contains(effect_type)) {
        effects.emplace(
            effect_type,
            effects::effect_stack{
                effect_application.source,
                (tick_t)std::min(effect_application.duration * effect_application.num_stacks,
                                 get_max_effect_duration(effect_type))});
    } else if (get_effect_stacking_type(effect_type) ==
               effects::effect_stacking_type::STACKING_DURATION) {
        effects.find(effect_type)->second.duration =
            std::min(effects.find(effect_type)->second.duration +
                         effect_application.duration * effect_application.num_stacks,
                     get_max_effect_duration(effect_type));
    } else {
        throw std::runtime_error("unexpected error!");
    }
}

static inline void remove_all_effect_stacks(effects::effect_type effect_type,
                                            entity_t entity,
                                            registry_t& registry) {
    registry.get<component::effects_component>(entity).effects.erase(effect_type);
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_EFFECT_UTILITIES_HPP

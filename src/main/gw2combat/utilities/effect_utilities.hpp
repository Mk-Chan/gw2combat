#ifndef GW2COMBAT_UTILITIES_EFFECT_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_EFFECT_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/effect/effect.hpp"
#include "gw2combat/effect/stacking.hpp"

#include "gw2combat/component/actor/effective_attributes.hpp"
#include "gw2combat/component/damage/multiplier/outgoing_condition_damage_multiplier.hpp"

namespace gw2combat::utils {

constexpr static inline std::array BOONS{
    effect::effect_t::AEGIS,
    effect::effect_t::ALACRITY,
    effect::effect_t::FURY,
    effect::effect_t::MIGHT,
    effect::effect_t::QUICKNESS,
    effect::effect_t::RESOLUTION,
    effect::effect_t::RESISTANCE,
    effect::effect_t::PROTECTION,
    effect::effect_t::REGENERATION,
    effect::effect_t::VIGOR,
    effect::effect_t::SWIFTNESS,
    effect::effect_t::STABILITY,
};

[[nodiscard]] static inline effect::stacking_t get_effect_stacking_type(
    effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::SPEAR_OF_JUSTICE:
        case effect::effect_t::CRIPPLED:
            return effect::stacking_t::STACKING_DURATION;
        case effect::effect_t::MIGHT:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return effect::stacking_t::STACKING_INTENSITY;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline size_t get_max_stored_stacks_of_effect_type(
    effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::SPEAR_OF_JUSTICE:
        case effect::effect_t::CRIPPLED:
            return 1;
        case effect::effect_t::MIGHT:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return 1500;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline size_t get_max_considered_stacks_of_effect_type(
    effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::SPEAR_OF_JUSTICE:
        case effect::effect_t::CRIPPLED:
            return 1;
        case effect::effect_t::SYMBOLIC_AVENGER:
            return 5;
        case effect::effect_t::MIGHT:
        case effect::effect_t::VULNERABILITY:
            return 25;
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return 1500;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline size_t get_max_effect_duration(effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::CRIPPLED:
            return 10;
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::MIGHT:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
            return 30;
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::SPEAR_OF_JUSTICE:
        case effect::effect_t::ARBITRARY_EFFECT:
            return 1'000'000'000;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline tick_t get_effective_effect_duration(
    tick_t duration,
    effect::effect_t effect_type,
    const component::effective_attributes& effective_attributes) {
    auto calculate_condition_duration = [&](double special_condition_duration_pct = 0.0) {
        return tick_t{
            (unsigned int)((double)duration *
                           (1.0 +
                            std::max(
                                effective_attributes[actor::attribute_t::CONDITION_DURATION_PCT],
                                special_condition_duration_pct) /
                                100.0))};
    };
    auto calculate_boon_duration = [&](double special_boon_duration_pct = 0.0) {
        return tick_t{
            (unsigned int)((double)duration *
                           (1.0 +
                            std::max(effective_attributes[actor::attribute_t::BOON_DURATION_PCT],
                                     special_boon_duration_pct) /
                                100.0))};
    };

    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::MIGHT:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
            return calculate_boon_duration();

        case effect::effect_t::CRIPPLED:
        case effect::effect_t::VULNERABILITY:
            return calculate_condition_duration();

        case effect::effect_t::BURNING:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::BURNING_DURATION_PCT]);
        case effect::effect_t::BLEEDING:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::BLEEDING_DURATION_PCT]);
        case effect::effect_t::TORMENT:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::TORMENT_DURATION_PCT]);
        case effect::effect_t::POISON:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::POISON_DURATION_PCT]);
        case effect::effect_t::CONFUSION:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::CONFUSION_DURATION_PCT]);

        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::SPEAR_OF_JUSTICE:
        case effect::effect_t::ARBITRARY_EFFECT:
            return tick_t{duration};

        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] constexpr static inline bool is_boon(effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::MIGHT:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
            return true;
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::CRIPPLED:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::SPEAR_OF_JUSTICE:
        case effect::effect_t::ARBITRARY_EFFECT:
            return false;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] constexpr static inline bool is_damaging_condition(effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::BINDING_BLADE:
            return true;
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::MIGHT:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::CRIPPLED:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::SPEAR_OF_JUSTICE:
        case effect::effect_t::ARBITRARY_EFFECT:
            return false;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline double calculate_condition_damage(effect::effect_t effect_type,
                                                              entity_t source_entity,
                                                              registry_t& registry) {
    if (!is_damaging_condition(effect_type)) {
        return 0.0;
    }

    auto&& [source_effective_attributes, outgoing_condition_damage_multiplier] =
        registry
            .get<component::effective_attributes, component::outgoing_condition_damage_multiplier>(
                source_entity);
    switch (effect_type) {
        case effect::effect_t::BURNING:
            return (131.0 +
                    0.155 * source_effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   outgoing_condition_damage_multiplier.multiplier *
                   outgoing_condition_damage_multiplier.burning_multiplier;
        case effect::effect_t::BLEEDING:
            return (22.0 +
                    0.06 * source_effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   outgoing_condition_damage_multiplier.multiplier *
                   outgoing_condition_damage_multiplier.bleeding_multiplier;
        case effect::effect_t::TORMENT:
            // FIXME: This is stationary-only torment damage for golem logs only
            return (31.8 +
                    0.09 * source_effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   outgoing_condition_damage_multiplier.multiplier *
                   outgoing_condition_damage_multiplier.torment_multiplier;
        case effect::effect_t::POISON:
            return (33.5 +
                    0.06 * source_effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   outgoing_condition_damage_multiplier.multiplier *
                   outgoing_condition_damage_multiplier.poison_multiplier;
        case effect::effect_t::CONFUSION:
            // FIXME: This is idle-only confusion damage for golem logs only
            return (11.0 +
                    0.03 * source_effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   outgoing_condition_damage_multiplier.multiplier *
                   outgoing_condition_damage_multiplier.confusion_multiplier;
        case effect::effect_t::BINDING_BLADE:
            return (160.0 + 0.2 * source_effective_attributes[actor::attribute_t::POWER]);
        default:
            throw std::runtime_error(fmt::format("missing damage formula for effect_type: {}",
                                                 utils::to_string(effect_type)));
    }
}

//[[nodiscard]] static inline double calculate_condition_damage(auto effect_type,
//                                                              auto effect_stacks,
//                                                              registry_t& registry) {
//    for (const auto& effect_stack : effect_stacks) {
//        return calculate_condition_damage(effect_type, effect_stack, registry);
//    }
//}
//
//[[nodiscard]] static inline size_t get_num_stacks_of_effect(
//    effect::effect_t effect_type,
//    const component::effects_component& effects_component) {
//    return std::min(effects_component.effects.count(effect_type),
//                    get_max_considered_stacks_of_effect_type(effect_type));
//}
//
//[[nodiscard]] static inline size_t get_num_stacks_of_effect(
//    effect::effect_t effect_type,
//    const component::effects_component* effects_component) {
//    if (effects_component == nullptr) {
//        return 0;
//    }
//    return get_num_stacks_of_effect(effect_type, *effects_component);
//}
//
//[[nodiscard]] static inline size_t get_num_stacks_of_effect(effect::effect_t effect_type,
//                                                            entity_t entity,
//                                                            registry_t& registry) {
//    auto effects_component_ptr = registry.try_get<component::effects_component>(entity);
//    return get_num_stacks_of_effect(effect_type, effects_component_ptr);
//}
//
//[[nodiscard]] static inline bool has_effect(effect::effect_t effect_type,
//                                            const component::effects_component& effects_component)
//                                            {
//    return effects_component.effects.contains(effect_type);
//}
//
//[[nodiscard]] static inline bool has_effect(effect::effect_t effect_type,
//                                            const component::effects_component* effects_component)
//                                            {
//    if (effects_component == nullptr) {
//        return false;
//    }
//    return has_effect(effect_type, *effects_component);
//}
//
//[[nodiscard]] static inline bool has_effect(effect::effect_t effect_type,
//                                            entity_t entity,
//                                            registry_t& registry) {
//    auto effects_component_ptr = registry.try_get<component::effects_component>(entity);
//    return has_effect(effect_type, effects_component_ptr);
//}
//
//[[nodiscard]] static inline bool is_effect_affected_by_incoming_multiplier(
//    effect::effect_t effect_type) {
//    return effect_type != effect::effect_t::BINDING_BLADE;
//}
//
// static inline void apply_effects(const effects::effect_application& effect_application,
//                                 component::effects_component& effects_component) {
//    effect::effect_t effect_type = effect_application.effect_type;
//    auto& effects = effects_component.effects;
//    if (get_effect_stacking_type(effect_type) ==
//        effects::effect_stacking_type::STACKING_INTENSITY) {
//        for (size_t i = 0; i < effect_application.num_stacks; ++i) {
//            if (get_num_stacks_of_effect(effect_type, effects_component) <
//                get_max_stored_stacks_of_effect_type(effect_type)) {
//                effects.emplace(
//                    effect_type,
//                    effects::effect_stack{effect_application.source,
//                    effect_application.duration});
//            }
//        }
//    } else if (!effects.contains(effect_type)) {
//        effects.emplace(
//            effect_type,
//            effects::effect_stack{
//                effect_application.source,
//                (tick_t)std::min(effect_application.duration * effect_application.num_stacks,
//                                 get_max_effect_duration(effect_type))});
//    } else if (get_effect_stacking_type(effect_type) ==
//               effects::effect_stacking_type::STACKING_DURATION) {
//        effects.find(effect_type)->second.duration =
//            std::min(effects.find(effect_type)->second.duration +
//                         effect_application.duration * effect_application.num_stacks,
//                     get_max_effect_duration(effect_type));
//    } else {
//        throw std::runtime_error("unexpected error!");
//    }
//}
//
// static inline void remove_all_effect_stacks(effect::effect_t effect_type,
//                                            entity_t entity,
//                                            registry_t& registry) {
//    registry.get<component::effects_component>(entity).effects.erase(effect_type);
//}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_EFFECT_UTILITIES_HPP

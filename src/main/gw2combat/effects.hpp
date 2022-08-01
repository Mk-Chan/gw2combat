#ifndef GW2COMBAT_EFFECT_HPP
#define GW2COMBAT_EFFECT_HPP

#include <numeric>

#include <entt/entt.hpp>

#include "types.hpp"

#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_condition_damage_multiplier.hpp"

namespace gw2combat::effects {

constexpr inline unsigned int effect_pulse_rate = 1'000;

template <typename Effect>
struct effect {
    using effect_type = Effect;

    explicit effect(entity_t source,
                    tick_t start_tick,
                    tick_t duration,
                    tick_t max_duration = 1'000'000'000)
        : source(source),
          start_tick(start_tick),
          duration(std::min(max_duration, duration)),
          last_damaging_tick(start_tick),
          max_duration(max_duration) {
    }

    [[nodiscard]] inline double damage_calculation(const entt::registry& registry,
                                                   tick_t current_tick) const {
        return get_child_type_const_ptr()->damage_calculation(registry, current_tick);
    }
    [[nodiscard]] inline bool is_expired(tick_t current_tick) const {
        return remaining(current_tick) <= 0.0;
    }
    [[nodiscard]] inline tick_t remaining(tick_t current_tick) const {
        return (start_tick + duration) - current_tick;
    }

    inline void for_each(
        registry_t& registry,
        tick_t current_tick,
        entity_t entity,
        const std::function<void(registry_t&, tick_t, entity_t, effect<effect_type>&)>& fn) {
        fn(registry, current_tick, entity, *this);
    }

    inline void update_last_damaging_tick(tick_t current_tick) {
        last_damaging_tick = current_tick;
    }
    inline void add(const effect<effect_type>& effect) {
        duration = std::min(max_duration, duration + effect.duration);
    }

    entity_t source;
    tick_t start_tick;
    tick_t duration;
    tick_t last_damaging_tick;
    tick_t max_duration;

   private:
    [[nodiscard]] inline effect_type const* get_child_type_const_ptr() const {
        return static_cast<effect_type const*>(this);
    }
    [[nodiscard]] inline effect_type* get_child_type_ptr() {
        return static_cast<effect_type*>(this);
    }
};

template <typename Effect>
struct stacking_effect {
    using effect_type = Effect;

    explicit stacking_effect(unsigned int max_considered_stacks)
        : max_considered_stacks(max_considered_stacks), max_stored_stacks(1500), stacks() {
    }
    stacking_effect(entity_t source,
                    size_t num_stacks,
                    tick_t start_tick,
                    tick_t duration,
                    size_t max_considered_stacks = 1500)
        : max_considered_stacks(max_considered_stacks), max_stored_stacks(1500), stacks() {
        size_t num_stacks_to_emplace = std::min(max_stored_stacks, num_stacks);
        for (size_t i = 0; i < num_stacks_to_emplace; ++i) {
            stacks.emplace_back(effect_type{source, start_tick, duration});
        }
    }

    [[nodiscard]] inline double damage_calculation(const entt::registry& registry,
                                                   tick_t current_tick) const {
        return std::accumulate(
            stacks.cbegin(), stacks.cend(), 0.0, [&](double accumulated, const auto& effect) {
                return accumulated + effect.damage_calculation(registry, current_tick);
            });
    }
    [[nodiscard]] inline bool is_expired(tick_t current_tick) const {
        return remaining(current_tick) <= 0.0;
    }
    [[nodiscard]] inline tick_t remaining(tick_t current_tick) const {
        return std::max_element(stacks.begin(), stacks.end(), [&](effect_type& effect) {
            return (effect.start_tick + effect.duration) - current_tick;
        });
    }
    [[nodiscard]] inline size_t num_stacks() const {
        return std::min(stacks.size(), max_considered_stacks);
    }

    inline void for_each(
        registry_t& registry,
        tick_t current_tick,
        entity_t entity,
        const std::function<void(registry_t&, tick_t, entity_t, effect_type&)>& fn) {
        for (effect_type& effect : stacks) {
            fn(registry, current_tick, entity, effect);
        }
    }

    inline void update_last_damaging_tick(tick_t current_tick) {
        for (auto& effect : stacks) {
            effect.update_last_damaging_tick(current_tick);
        }
    }
    inline void remove_expired_effects(tick_t current_tick) {
        std::erase_if(stacks, [&](effect_type& effect) { return effect.is_expired(current_tick); });
    }
    inline void add(const effect_type& effect) {
        if (stacks.size() < max_stored_stacks) {
            stacks.emplace_back(effect);
        }
    }

   protected:
    size_t max_considered_stacks;
    size_t max_stored_stacks;
    std::vector<effect_type> stacks;
};

struct aegis : effect<aegis> {
    using effect<aegis>::effect;
};
struct alacrity : effect<alacrity> {
    using effect<alacrity>::effect;
};
struct fury : effect<fury> {
    using effect<fury>::effect;
};
struct might : effect<might> {
    using effect<might>::effect;
};
struct quickness : effect<quickness> {
    using effect<quickness>::effect;
};
struct resolution : effect<resolution> {
    using effect<resolution>::effect;
};
struct vulnerability : effect<vulnerability> {
    using effect<vulnerability>::effect;
};
struct burning : effect<burning> {
    using effect<burning>::effect;

    [[nodiscard]] inline double damage_calculation(const entt::registry& registry,
                                                   tick_t current_tick) const {
        auto&& [source_effective_attributes, source_outgoing_condition_damage_multiplier] =
            registry.get<component::effective_attributes,
                         component::outgoing_condition_damage_multiplier>(source);

        double damage = (131.0 + 0.155 * source_effective_attributes.condition_damage) *
                        source_outgoing_condition_damage_multiplier.multiplier;

        return damage * (current_tick - last_damaging_tick) / effect_pulse_rate;
    }
};
struct bleeding : effect<bleeding> {
    using effect<bleeding>::effect;

    [[nodiscard]] inline double damage_calculation(const entt::registry& registry,
                                                   tick_t current_tick) const {
        auto&& [source_effective_attributes, source_outgoing_condition_damage_multiplier] =
            registry.get<component::effective_attributes,
                         component::outgoing_condition_damage_multiplier>(source);

        double damage = (22.0 + 0.06 * source_effective_attributes.condition_damage) *
                        source_outgoing_condition_damage_multiplier.multiplier;

        return damage * (current_tick - last_damaging_tick) / effect_pulse_rate;
    }
};

struct binding_blade : effect<binding_blade> {
    using effect<binding_blade>::effect;

    [[nodiscard]] inline double damage_calculation(const entt::registry& registry,
                                                   tick_t current_tick) const {
        auto source_effective_attributes = registry.get<component::effective_attributes>(source);

        double damage = (160.0 + 0.2 * source_effective_attributes.power);

        return damage * (current_tick - last_damaging_tick) / effect_pulse_rate;
    }
};

struct symbolic_avenger : effect<symbolic_avenger> {
    using effect<symbolic_avenger>::effect;
};

}  // namespace gw2combat::effects

#endif  // GW2COMBAT_EFFECT_HPP

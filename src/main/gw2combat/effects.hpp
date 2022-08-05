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
struct effect_old {
    using effect_type = Effect;

    explicit effect_old(entity_t source,
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
        if (current_tick > (start_tick + duration)) {
            return 0;
        }
        return (start_tick + duration) - current_tick;
    }

    inline void for_each(
        registry_t& registry,
        tick_t current_tick,
        entity_t entity,
        const std::function<void(registry_t&, tick_t, entity_t, effect_old<effect_type>&)>& fn) {
        fn(registry, current_tick, entity, *this);
    }

    inline void update_last_damaging_tick(tick_t current_tick) {
        last_damaging_tick = current_tick;
    }
    inline void add(const effect_old<effect_type>& effect) {
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
            return effect.remaining(current_tick);
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
    inline void add(const effect_type& effect, size_t num_stacks) {
        for (size_t i = 0; i < num_stacks; ++i) {
            if (stacks.size() < max_stored_stacks) {
                stacks.emplace_back(effect);
            }
        }
    }

   protected:
    size_t max_considered_stacks;
    size_t max_stored_stacks;
    std::vector<effect_type> stacks;
};

struct aegis : effect_old<aegis> {
    using effect_old<aegis>::effect_old;
};
struct alacrity : effect_old<alacrity> {
    using effect_old<alacrity>::effect_old;
};
struct fury : effect_old<fury> {
    using effect_old<fury>::effect_old;
};
struct might : effect_old<might> {
    using effect_old<might>::effect_old;
};
struct quickness : effect_old<quickness> {
    using effect_old<quickness>::effect_old;
};
struct resolution : effect_old<resolution> {
    using effect_old<resolution>::effect_old;
};
struct vulnerability : effect_old<vulnerability> {
    using effect_old<vulnerability>::effect_old;
};
struct burning : effect_old<burning> {
    using effect_old<burning>::effect_old;

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
struct bleeding : effect_old<bleeding> {
    using effect_old<bleeding>::effect_old;

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

struct binding_blade : effect_old<binding_blade> {
    using effect_old<binding_blade>::effect_old;

    [[nodiscard]] inline double damage_calculation(const entt::registry& registry,
                                                   tick_t current_tick) const {
        auto source_effective_attributes = registry.get<component::effective_attributes>(source);

        double damage = (160.0 + 0.2 * source_effective_attributes.power);

        return damage * (current_tick - last_damaging_tick) / effect_pulse_rate;
    }
};

struct symbolic_avenger : effect_old<symbolic_avenger> {
    using effect_old<symbolic_avenger>::effect_old;
};

struct virtue_of_justice : effect_old<virtue_of_justice> {
    using effect_old<virtue_of_justice>::effect_old;
};
struct inspiring_virtue : effect_old<inspiring_virtue> {
    using effect_old<inspiring_virtue>::effect_old;
};

enum class effect_type : std::uint32_t
{
    BURNING,
    BLEEDING,

    BINDING_BLADE,
    VIRTUE_OF_JUSTICE,
};

struct effect_application {
    effects::effect_type effect_type;
    size_t num_stacks;
    tick_t duration;
    entity_t source;
};

NLOHMANN_JSON_SERIALIZE_ENUM(effect_type,
                             {
                                 {effect_type::BURNING, "BURNING"},
                                 {effect_type::BLEEDING, "BLEEDING"},

                                 {effect_type::BINDING_BLADE, "BINDING_BLADE"},
                                 {effect_type::VIRTUE_OF_JUSTICE, "VIRTUE_OF_JUSTICE"},
                             })

}  // namespace gw2combat::effects

#endif  // GW2COMBAT_EFFECT_HPP

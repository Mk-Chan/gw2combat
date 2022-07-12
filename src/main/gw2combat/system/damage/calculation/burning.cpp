#include "condition_damage_calculations.hpp"

namespace gw2combat::system {

double calculate_burning_damage(context& ctx,
                                const component::burning& burning,
                                const tick_t ticks_to_consider) {
    double accumulated_potency = 0.0;
    for (const effect& effect : burning.stacks) {
        auto&& [source_effective_attributes, source_outgoing_condition_damage_multiplier] =
            ctx.registry.get<component::effective_attributes,
                             component::outgoing_condition_damage_multiplier>(effect.source);

        double effect_potency = source_effective_attributes.condition_damage *
                                source_outgoing_condition_damage_multiplier.multiplier;

        accumulated_potency += effect_potency;
    }
    double burning_damage = 131.0 * (double)burning.stacks.size() + 0.155 * accumulated_potency;

    return burning_damage * ticks_to_consider / component::pulse_conditions::pulse_rate;
}

}  // namespace gw2combat::system

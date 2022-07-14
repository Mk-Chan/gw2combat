#include "condition_damage_calculations.hpp"

namespace gw2combat::system {

double calculate_burning_damage(context& ctx,
                                const effect& burning_effect,
                                tick_t ticks_to_consider) {
    auto&& [source_effective_attributes, source_outgoing_condition_damage_multiplier] =
        ctx.registry
            .get<component::effective_attributes, component::outgoing_condition_damage_multiplier>(
                burning_effect.source);

    double burning_damage = (131.0 + 0.155 * source_effective_attributes.condition_damage) *
                            source_outgoing_condition_damage_multiplier.multiplier;

    return burning_damage * ticks_to_consider / component::pulse_conditions::pulse_rate;
}

}  // namespace gw2combat::system

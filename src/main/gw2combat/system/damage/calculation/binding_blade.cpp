#include "condition_damage_calculations.hpp"

namespace gw2combat::system {

double calculate_binding_blade_damage(context& ctx,
                                      const effect& effect,
                                      tick_t ticks_to_consider) {
    auto source_effective_attributes =
        ctx.registry.get<component::effective_attributes>(effect.source);

    double damage = (160.0 + 0.2 * source_effective_attributes.power);

    return damage * ticks_to_consider / component::pulse_conditions::pulse_rate;
}

}  // namespace gw2combat::system

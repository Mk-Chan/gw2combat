#include "gw2combat/system/system.hpp"

#include <numeric>

#include <spdlog/spdlog.h>

#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/condition/vulnerability.hpp"
#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/effective_incoming_damage.hpp"
#include "gw2combat/component/outgoing_condition_damage_multiplier.hpp"
#include "gw2combat/component/pulse_condition.hpp"

namespace gw2combat::system {

void burning_damage_calculation(context& ctx) {
    ctx.registry.view<component::burning>().each([&](const entt::entity entity,
                                                     component::burning& burning) {
        if (ctx.registry.any_of<component::pulse_condition>(*singleton_entity)) {
            double accumulated_potency = 0.0;
            for (effect& effect : burning.stacks) {
                auto&& [source_effective_attributes, source_outgoing_condition_damage_multiplier] =
                    ctx.registry.get<component::effective_attributes,
                                     component::outgoing_condition_damage_multiplier>(
                        effect.source);

                double effect_potency = source_effective_attributes.condition_damage *
                                        source_outgoing_condition_damage_multiplier.multiplier;
                if (effect.is_expired) {
                    double damage_reduction_multiplier =
                        (ctx.current_tick - effect.last_ticked) /
                        (double)component::pulse_condition::pulse_rate;
                    effect_potency *= damage_reduction_multiplier;
                }

                // Record that this effect was applied on this tick for partial effect calculations
                effect.last_ticked = ctx.current_tick;

                accumulated_potency += effect_potency;
            }
            double burning_damage =
                131.0 * (double)burning.stacks.size() + 0.155 * accumulated_potency;

            double vulnerability_multiplier = [&]() {
                auto vulnerability_ptr = ctx.registry.try_get<component::vulnerability>(entity);
                if (vulnerability_ptr == nullptr) {
                    return 1.0;
                }
                return 1.0 + ((double)(vulnerability_ptr->stacks.size()) * 0.01);
            }();

            double effective_burning_damage = burning_damage * vulnerability_multiplier;

            auto& effective_incoming_damage =
                ctx.registry.get_or_emplace<component::effective_incoming_damage>(
                    entity, component::effective_incoming_damage{0});
            effective_incoming_damage.value += (unsigned int)std::round(effective_burning_damage);

            spdlog::info("entity: {}, incoming condition damage: {}, effective incoming damage: {}",
                         static_cast<std::uint32_t>(entity),
                         effective_burning_damage,
                         effective_incoming_damage.value);
        }
    });
}

}  // namespace gw2combat::system

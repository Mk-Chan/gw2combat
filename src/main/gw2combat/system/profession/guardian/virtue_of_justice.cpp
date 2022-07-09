#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/combat_stats.hpp"
#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/outgoing_condition_application.hpp"
#include "gw2combat/component/profession/guardian/virtue_of_justice.hpp"
#include "gw2combat/component/targeting.hpp"

namespace gw2combat::system {

void virtue_of_justice(context& ctx) {
    ctx.registry
        .view<component::combat_stats,
              component::effective_attributes,
              component::virtue_of_justice>()
        .each([&](const entt::entity entity,
                  const component::combat_stats& combat_stats,
                  const component::effective_attributes& effective_attributes,
                  component::virtue_of_justice& virtue_of_justice) {
            while (combat_stats.num_hits >= virtue_of_justice.next_burning_application_hit_count) {
                virtue_of_justice.next_burning_application_hit_count +=
                    virtue_of_justice.number_of_ticks_for_burning_application;
                double condition_duration_multiplier =
                    (1.0 + effective_attributes.condition_duration_pct / 100.0);
                tick_t condition_end_tick =
                    ctx.current_tick +
                    tick_t{(unsigned int)(2'000.0 * condition_duration_multiplier)};

                auto& outgoing_condition_application =
                    ctx.registry.get_or_emplace<component::outgoing_condition_application>(entity);
                outgoing_condition_application.append_burning_effects(
                    effect{entity, condition_end_tick}, 1);

                spdlog::info("tick: {}, entity: {}, outgoing burning from virtue_of_justice",
                             ctx.current_tick,
                             static_cast<std::uint32_t>(entity));
            }
        });
}

}  // namespace gw2combat::system

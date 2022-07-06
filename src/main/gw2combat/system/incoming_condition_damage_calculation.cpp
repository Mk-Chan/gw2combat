#include "system.hpp"

#include <numeric>

#include <spdlog/spdlog.h>

#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/condition_tick_status.hpp"
#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/effective_incoming_damage.hpp"

namespace gw2combat::system {

void incoming_condition_damage_calculation(context& ctx) {
    ctx.registry.view<component::burning>().each(
        [&](const entt::entity entity, const component::burning& burning) {
            // TODO: Maybe implement this filter by adding a component to all entities that are the
            //       subject of condition damage when a condition tick is occurring?
            if (!ctx.registry.get<component::condition_tick_status>(*singleton_entity)
                     .apply_condition_damage) {
                return;
            }

            double burning_damage =
                131.0 * (double)burning.stacks.size() +
                0.155 * std::accumulate(
                            burning.stacks.cbegin(),
                            burning.stacks.cend(),
                            0.0,
                            [&](const double accumulated, const effect& next) {
                                double source_condition_damage =
                                    ctx.registry.get<component::effective_attributes>(next.source)
                                        .condition_damage;
                                return accumulated + source_condition_damage;
                            });
            spdlog::info("burning_damage: {}", burning_damage);

            auto incoming_condition_damage = (unsigned int)(burning_damage);

            auto& effective_incoming_damage =
                ctx.registry.get_or_emplace<component::effective_incoming_damage>(
                    entity, component::effective_incoming_damage{0});
            effective_incoming_damage.value += incoming_condition_damage;

            spdlog::info("entity: {}, incoming condition damage: {}, effective incoming damage: {}",
                         static_cast<std::uint32_t>(entity),
                         incoming_condition_damage,
                         effective_incoming_damage.value);
        });
}

}  // namespace gw2combat::system

#include "gw2combat/system/system.hpp"

#include <numeric>

#include <spdlog/spdlog.h>

#include "gw2combat/component/condition/vulnerability.hpp"
#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/effective_incoming_damage.hpp"
#include "gw2combat/component/incoming_strike_damage.hpp"

namespace gw2combat::system {

// NOTE: Easily optimized with some group-by logic if required
void strike_damage_calculation(context& ctx) {
    ctx.registry.view<component::incoming_strike_damage, component::effective_attributes>().each(
        [&](const entt::entity entity,
            const component::incoming_strike_damage& incoming_damage,
            const component::effective_attributes& effective_attributes) {
            double accumulated_incoming_damage =
                std::accumulate(incoming_damage.strikes.begin(),
                                incoming_damage.strikes.end(),
                                0.0,
                                [&](const double accumulated, const strike& next) {
                                    return accumulated + next.damage;
                                });

            double vulnerability_multiplier = [&]() {
                auto vulnerability_ptr = ctx.registry.try_get<component::vulnerability>(entity);
                if (vulnerability_ptr == nullptr) {
                    return 1.0;
                }
                return 1.0 + ((double)(vulnerability_ptr->stacks.size()) * 0.01);
            }();

            auto incoming_strike_damage =
                (unsigned int)std::round(accumulated_incoming_damage * vulnerability_multiplier /
                                         effective_attributes.armor);
            auto& effective_incoming_damage =
                ctx.registry.get_or_emplace<component::effective_incoming_damage>(
                    entity, component::effective_incoming_damage{0});
            effective_incoming_damage.value += incoming_strike_damage;

            spdlog::info("entity: {}, incoming strike damage: {}, effective incoming damage: {}",
                         static_cast<std::uint32_t>(entity),
                         incoming_strike_damage,
                         effective_incoming_damage.value);
        });
}

}  // namespace gw2combat::system

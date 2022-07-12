#include <spdlog/spdlog.h>

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/damage/outgoing_condition_application.hpp"

namespace gw2combat::system {

void incoming_condition_application(context& ctx) {
    ctx.registry.view<component::outgoing_condition_application>().each(
        [&](const entt::entity entity,
            const component::outgoing_condition_application& outgoing_condition_application) {
            auto targeting_ptr = ctx.registry.try_get<component::targeting>(entity);
            if (targeting_ptr) {
                if (!outgoing_condition_application.burning_effects.empty()) {
                    auto& target_burning = ctx.registry.get_or_emplace<component::burning>(
                        targeting_ptr->entity, component::burning{});
                    for (auto& burning_effect : outgoing_condition_application.burning_effects) {
                        target_burning.stacks.push_back(burning_effect);
                    }
                    spdlog::info("tick: {}, added {} burning effects, target: {}",
                                 ctx.current_tick,
                                 outgoing_condition_application.burning_effects.size(),
                                 static_cast<std::uint32_t>(targeting_ptr->entity));
                }
                // NOTE: Bleeding, torment, confusion etc...
            }
        });
}

}  // namespace gw2combat::system

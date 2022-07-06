#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/profession/guardian/virtue_of_justice.hpp"
#include "gw2combat/component/targeting.hpp"

namespace gw2combat::system {

void outgoing_condition_application(context& ctx) {
    ctx.registry.view<component::virtue_of_justice>().each(
        [&](const entt::entity entity, const component::virtue_of_justice& virtue_of_justice) {
            if (virtue_of_justice.apply_burning_on_tick == ctx.current_tick) {
                spdlog::info("applying virtue of justice");
                auto target_ptr = ctx.registry.try_get<component::targeting>(entity);
                if (target_ptr) {
                    auto& burning = ctx.registry.get_or_emplace<component::burning>(
                        target_ptr->entity, component::burning{});
                    burning.stacks.emplace(effect{
                        entity,
                        ctx.current_tick +
                            tick_t{
                                2'000 *
                                (unsigned int)(1.0 +
                                               ctx.registry
                                                       .get<component::effective_attributes>(entity)
                                                       .condition_duration_pct /
                                                   100.0)}});
                }
            }
        });
}

}  // namespace gw2combat::system

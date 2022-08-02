#include "gw2combat/system/system.hpp"

#include <entt/entt.hpp>

#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/source_entity.hpp"
#include "gw2combat/component/gear/sigil/sigil_bursting.hpp"

namespace gw2combat::system {

void outgoing_condition_damage_multiplier_calculation(registry_t& registry, tick_t) {
    registry.view<component::sigil_bursting>().each([&](entity_t entity) {
        auto& outgoing_condition_damage_multiplier =
            registry.emplace<component::outgoing_condition_damage_multiplier>(entity);
        outgoing_condition_damage_multiplier.multiplier =
            1.0 + component::sigil_bursting::outgoing_condition_damage_increase;
    });
    registry.view<component::is_actor>(entt::exclude<component::sigil_bursting>)
        .each([&](entity_t entity) {
            registry.emplace<component::outgoing_condition_damage_multiplier>(entity);
        });
    registry.view<component::source_entity>().each(
        [&](entity_t entity, const component::source_entity& outgoing_damage_source) {
            auto source_entity = utils::get_source_entity(outgoing_damage_source.entity, registry);
            auto& source_outgoing_condition_damage_multiplier =
                registry.get<component::outgoing_condition_damage_multiplier>(source_entity);
            registry.emplace_or_replace<component::outgoing_condition_damage_multiplier>(
                entity, source_outgoing_condition_damage_multiplier);
        });
}

}  // namespace gw2combat::system

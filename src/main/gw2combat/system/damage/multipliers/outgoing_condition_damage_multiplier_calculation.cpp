#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/source_entity.hpp"

namespace gw2combat::system {

void outgoing_condition_damage_multiplier_calculation(registry_t& registry, tick_t) {
    registry.view<component::is_actor>().each([&](entity_t entity) {
        auto& outgoing_condition_damage_multiplier =
            registry.get_or_emplace<component::outgoing_condition_damage_multiplier>(entity);
        if (utils::has_sigil(weapon_sigil::BURSTING, entity, registry)) {
            outgoing_condition_damage_multiplier.multiplier *= 1.05;
        }
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

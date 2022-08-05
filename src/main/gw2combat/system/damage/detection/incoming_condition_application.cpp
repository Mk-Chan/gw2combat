#include "gw2combat/system/system.hpp"

#include "gw2combat/component/damage/incoming_condition_application.hpp"
#include "gw2combat/component/effect_components.hpp"

namespace gw2combat::system {

void incoming_condition_application(registry_t& registry, tick_t current_tick) {
    registry.view<component::incoming_condition_application>().each(
        [&](entity_t entity,
            const component::incoming_condition_application& incoming_condition_application) {
            for (auto& effect_application : incoming_condition_application.effect_applications) {
                if (effect_application.effect_type == effects::applied_effect_type::BURNING) {
                    auto& burning = registry.get_or_emplace<component::burning>(entity);
                    burning.effect.add(
                        effects::burning{
                            effect_application.source, current_tick, effect_application.duration},
                        effect_application.num_stacks);
                } else if (effect_application.effect_type ==
                           effects::applied_effect_type::BLEEDING) {
                    auto& bleeding = registry.get_or_emplace<component::bleeding>(entity);
                    bleeding.effect.add(
                        effects::bleeding{
                            effect_application.source, current_tick, effect_application.duration},
                        effect_application.num_stacks);
                } else if (effect_application.effect_type ==
                           effects::applied_effect_type::BINDING_BLADE) {
                    registry.emplace_or_replace<component::binding_blade>(
                        entity,
                        effects::binding_blade{
                            effect_application.source, current_tick, effect_application.duration});
                } else if (effect_application.effect_type ==
                           effects::applied_effect_type::VIRTUE_OF_JUSTICE) {
                    registry.emplace_or_replace<component::virtue_of_justice_effect>(
                        entity,
                        effects::virtue_of_justice{
                            effect_application.source, current_tick, effect_application.duration});
                }
            }
            // NOTE: Torment, confusion etc...
        });
}

}  // namespace gw2combat::system

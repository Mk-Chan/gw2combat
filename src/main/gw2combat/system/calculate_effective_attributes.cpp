#include "system.hpp"

#include "gw2combat/component/actor/effective_attributes.hpp"
#include "gw2combat/component/actor/static_attributes.hpp"
#include "gw2combat/component/attribute_modifications.hpp"
#include "gw2combat/component/cooldown.hpp"
#include "gw2combat/component/owner_actor.hpp"

namespace gw2combat::system {

void calculate_effective_attributes(registry_t& registry) {
    registry.view<component::static_attributes>().each(
        [&](entity_t entity, const component::static_attributes& static_attributes) {
            auto& effective_attributes = registry.emplace<component::effective_attributes>(entity);
            effective_attributes.attribute_value_map = static_attributes.attribute_value_map;
        });
    registry
        .view<component::attribute_modifications_component, component::owner_actor>(
            entt::exclude<component::cooldown>)
        .each([&](const component::attribute_modifications_component&
                      attribute_modifications_component,
                  const component::owner_actor& owner_actor) {
            auto& effective_attributes =
                registry.get<component::effective_attributes>(owner_actor.entity);
            for (auto& attribute_modification :
                 attribute_modifications_component.attribute_modifications) {
                auto& attribute =
                    effective_attributes.attribute_value_map[attribute_modification.attribute];
                attribute =
                    attribute * attribute_modification.multiplier + attribute_modification.addend;
            }
        });
}

}  // namespace gw2combat::system

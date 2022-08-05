#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/effect_components.hpp"

namespace gw2combat::system {

template <typename EffectComponent>
void expire_end_of_life_effects(registry_t& registry, tick_t current_tick) {
    registry.view<EffectComponent>().each([&](entity_t entity, EffectComponent& effect_component) {
        if constexpr (std::is_same_v<
                          decltype(effect_component.effect_old),
                          effects::stacking_effect<typename EffectComponent::effect_type>>) {
            effect_component.effect_old.remove_expired_effects(current_tick);
            if (effect_component.effect_old.num_stacks() == 0) {
                registry.remove<EffectComponent>(entity);
            }
        } else if (effect_component.effect_old.is_expired(current_tick)) {
            registry.remove<EffectComponent>(entity);
        }
    });
}

void expire_non_damaging_effects(registry_t& registry, tick_t current_tick) {
    expire_end_of_life_effects<component::aegis>(registry, current_tick);
    expire_end_of_life_effects<component::alacrity>(registry, current_tick);
    expire_end_of_life_effects<component::fury>(registry, current_tick);
    expire_end_of_life_effects<component::might>(registry, current_tick);
    expire_end_of_life_effects<component::quickness>(registry, current_tick);
    expire_end_of_life_effects<component::resolution>(registry, current_tick);

    expire_end_of_life_effects<component::vulnerability>(registry, current_tick);
    expire_end_of_life_effects<component::symbolic_avenger_effect>(registry, current_tick);
    expire_end_of_life_effects<component::virtue_of_justice_effect>(registry, current_tick);
    expire_end_of_life_effects<component::inspiring_virtue_effect>(registry, current_tick);
}

}  // namespace gw2combat::system

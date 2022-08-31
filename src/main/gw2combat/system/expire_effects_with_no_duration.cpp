#include "system.hpp"

#include "gw2combat/component/actor/alacrity.hpp"
#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/quickness.hpp"
#include "gw2combat/component/actor/unique_effects_component.hpp"
#include "gw2combat/component/effect/duration.hpp"
#include "gw2combat/component/effect/is_effect.hpp"
#include "gw2combat/component/effect/is_unique_effect.hpp"

namespace gw2combat::system {

void clean_invalid_effect_entities(registry_t& registry) {
    registry.view<component::effects_component>().each(
        [&](entity_t entity, component::effects_component& effects_component) {
            for (auto iter = effects_component.effect_entities.begin();
                 iter != effects_component.effect_entities.end();) {
                if (!registry.valid(iter->entity)) {
                    if (iter->effect == effect::effect_t::QUICKNESS) {
                        registry.remove<component::quickness>(entity);
                    } else if (iter->effect == effect::effect_t::ALACRITY) {
                        registry.remove<component::alacrity>(entity);
                    }
                    iter = effects_component.effect_entities.erase(iter);
                } else {
                    ++iter;
                }
            }
        });
}

void expire_non_damaging_effects_with_no_duration(registry_t& registry) {
    registry.view<component::duration_expired, component::is_effect>().each(
        [&](entity_t entity, const component::is_effect& is_effect) {
            if (utils::is_damaging_condition(is_effect.effect)) {
                return;
            }
            registry.destroy(entity);
        });
    clean_invalid_effect_entities(registry);

    registry.view<component::duration_expired, component::is_unique_effect>().each(
        [&](entity_t entity, const component::is_unique_effect&) { registry.destroy(entity); });
    registry.view<component::unique_effects_component>().each(
        [&](component::unique_effects_component& unique_effects_component) {
            for (auto iter = unique_effects_component.unique_effect_entities.begin();
                 iter != unique_effects_component.unique_effect_entities.end();) {
                if (!registry.valid(iter->entity)) {
                    iter = unique_effects_component.unique_effect_entities.erase(iter);
                } else {
                    ++iter;
                }
            }
        });
}

void expire_damaging_effects_with_no_duration(registry_t& registry) {
    registry.view<component::duration_expired, component::is_effect>().each(
        [&](entity_t entity, const component::is_effect& is_effect) {
            if (!utils::is_damaging_condition(is_effect.effect)) {
                return;
            }
            // spdlog::info("MAU!!!!!! effect expired, buffering damage");
            buffer_condition_damage(registry, entity);
            registry.destroy(entity);
        });
    clean_invalid_effect_entities(registry);
}

}  // namespace gw2combat::system

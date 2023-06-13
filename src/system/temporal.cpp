#include "temporal.hpp"

#include "component/actor/casting_skills.hpp"
#include "component/lifecycle/destroy_entity.hpp"
#include "component/skill/ammo.hpp"
#include "component/temporal/animation_component.hpp"
#include "component/temporal/cooldown_component.hpp"
#include "component/temporal/duration_component.hpp"
#include "component/temporal/has_alacrity.hpp"
#include "component/temporal/has_quickness.hpp"

#include "utils/actor_utils.hpp"

namespace gw2combat::system {

void progress_animations(registry_t& registry) {
    registry.view<component::animation_component>().each(
        [&](entity_t entity, component::animation_component& animation) {
            if (animation.duration[0] == 0) {
                registry.emplace<component::animation_expired>(entity);
                return;
            }

            bool has_quickness =
                registry.any_of<component::has_quickness>(utils::get_owner(entity, registry));
            animation.progress[0] += !has_quickness;
            animation.progress[1] += has_quickness;

            int no_quickness_progress_pct = animation.progress[0] * 100 / animation.duration[0];
            int quickness_progress_pct = animation.progress[1] * 100 / animation.duration[1];

            if (no_quickness_progress_pct + quickness_progress_pct >= 100) {
                registry.emplace<component::animation_expired>(entity);
            }
        });
}

void progress_cooldowns(registry_t& registry) {
    registry.view<component::cooldown_component>().each(
        [&](entity_t entity, component::cooldown_component& cooldown) {
            if (cooldown.duration[0] == 0) {
                auto& ammo = registry.get<component::ammo>(entity);
                ammo.current_ammo = ammo.max_ammo;
                registry.emplace<component::cooldown_expired>(entity);
                return;
            }

            bool has_alacrity =
                registry.any_of<component::has_alacrity>(utils::get_owner(entity, registry));
            cooldown.progress[0] += !has_alacrity;
            cooldown.progress[1] += has_alacrity;

            int no_alacrity_progress_pct = cooldown.progress[0] * 100 / cooldown.duration[0];
            int alacrity_progress_pct = cooldown.progress[1] * 100 / cooldown.duration[1];

            if (no_alacrity_progress_pct + alacrity_progress_pct >= 100) {
                auto ammo = registry.try_get<component::ammo>(entity);
                if (ammo) {
                    ++ammo->current_ammo;
                    if (ammo->current_ammo == ammo->max_ammo) {
                        registry.emplace<component::cooldown_expired>(entity);
                    } else {
                        cooldown.progress = {0, 0};
                    }
                } else {
                    registry.emplace<component::cooldown_expired>(entity);
                }
            }
        });
}

void progress_durations(registry_t& registry) {
    registry.view<component::duration_component>().each(
        [&](entity_t entity, component::duration_component& duration) {
            ++duration.progress;
            if (duration.progress >= duration.duration) {
                registry.emplace<component::duration_expired>(entity);
            }
        });
}

void progress_casting_skills(registry_t& registry) {
    registry.view<component::casting_skills_component>(entt::exclude<component::has_quickness>)
        .each([&](component::casting_skills_component& casting_skills_component) {
            for (auto& skill_state : casting_skills_component.skills) {
                ++skill_state.pulse_progress[0];
                ++skill_state.strike_progress[0];
            }
        });
    registry.view<component::casting_skills_component, component::has_quickness>().each(
        [&](component::casting_skills_component& casting_skills_component) {
            for (auto& skill_state : casting_skills_component.skills) {
                ++skill_state.pulse_progress[1];
                ++skill_state.strike_progress[1];
            }
        });
}

void cleanup_expired_components(registry_t& registry) {
    registry.view<component::animation_expired>().each(
        [&](entity_t entity) { registry.remove<component::animation_component>(entity); });
    registry.view<component::cooldown_expired>().each(
        [&](entity_t entity) { registry.remove<component::cooldown_component>(entity); });
    registry.view<component::duration_expired>().each([&](entity_t entity) {
        if (registry.any_of<component::is_effect>(entity)) {
            auto& is_effect = registry.get<component::is_effect>(entity);
            if (is_effect.effect == actor::effect_t::QUICKNESS) {
                registry.remove<component::has_quickness>(utils::get_owner(entity, registry));
            } else if (is_effect.effect == actor::effect_t::ALACRITY) {
                registry.remove<component::has_alacrity>(utils::get_owner(entity, registry));
            }
        }
        registry.emplace_or_replace<component::destroy_entity>(entity);
    });
}

}  // namespace gw2combat::system

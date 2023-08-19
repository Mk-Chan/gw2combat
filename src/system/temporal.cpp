#include "temporal.hpp"

#include "component/actor/finished_casting_skills.hpp"
#include "component/actor/skills_actions_component.hpp"
#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"
#include "component/lifecycle/destroy_entity.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/animation_component.hpp"
#include "component/temporal/cooldown_component.hpp"
#include "component/temporal/duration_component.hpp"
#include "component/temporal/has_alacrity.hpp"
#include "component/temporal/has_quickness.hpp"

#include "utils/actor_utils.hpp"
#include "utils/condition_utils.hpp"
#include "utils/side_effect_utils.hpp"

namespace gw2combat::system {

void progress_animations(registry_t& registry) {
    registry.view<component::animation_component>().each(
        [&](entity_t entity, component::animation_component& animation) {
            if (animation.duration[0] == 0) {
                registry.emplace_or_replace<component::animation_expired>(entity);
                utils::finish_casting_skill(animation.skill_entity, entity, registry);
                return;
            }

            bool has_quickness =
                registry.any_of<component::has_quickness>(utils::get_owner(entity, registry));
            animation.progress[0] += !has_quickness;
            animation.progress[1] += has_quickness;

            int no_quickness_progress_pct = animation.progress[0] * 100 / animation.duration[0];
            int quickness_progress_pct = animation.progress[1] * 100 / animation.duration[1];

            if (no_quickness_progress_pct + quickness_progress_pct >= 100) {
                registry.emplace_or_replace<component::animation_expired>(entity);
                utils::finish_casting_skill(animation.skill_entity, entity, registry);
            }
        });

    registry.view<component::finished_casting_skills>().each([&](entity_t actor_entity,
                                                                 component::finished_casting_skills&
                                                                     finished_casting_skills) {
        for (auto finished_casting_skill_entity : finished_casting_skills.skill_entities) {
            auto& skill_configuration =
                registry.get<component::is_skill>(finished_casting_skill_entity)
                    .skill_configuration;
            spdlog::info("[{}] {}: finishing skill {}",
                         utils::get_current_tick(registry),
                         utils::get_entity_name(actor_entity, registry),
                         skill_configuration.skill_key);

            if (!skill_configuration.equip_bundle.empty()) {
                registry.emplace<component::bundle_component>(
                    actor_entity, component::bundle_component{skill_configuration.equip_bundle});
                registry.emplace_or_replace<component::equipped_bundle>(
                    actor_entity, skill_configuration.equip_bundle);
                spdlog::info("[{}] {}: equipped bundle {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(actor_entity, registry),
                             skill_configuration.equip_bundle);
            } else if (skill_configuration.skill_key == "Weapon Swap") {
                if (auto bundle_ptr = registry.try_get<component::bundle_component>(actor_entity);
                    bundle_ptr) {
                    registry.emplace_or_replace<component::dropped_bundle>(actor_entity,
                                                                           bundle_ptr->name);
                    registry.remove<component::bundle_component>(actor_entity);
                    spdlog::info("[{}] {}: dropped bundle {}",
                                 utils::get_current_tick(registry),
                                 utils::get_entity_name(actor_entity, registry),
                                 bundle_ptr->name);
                } else {
                    if (!registry.any_of<component::current_weapon_set>(actor_entity)) {
                        throw std::runtime_error("no equipped_weapon_set on entity");
                    }
                    auto& equipped_weapons =
                        registry.get<component::equipped_weapons>(actor_entity);
                    if (equipped_weapons.weapons.size() == 1) {
                        throw std::runtime_error(
                            "cannot weapon swap when there is only 1 weapon set equipped");
                    }

                    auto current_set =
                        registry.get<component::current_weapon_set>(actor_entity).set;
                    if (current_set == actor::weapon_set::SET_1) {
                        registry.replace<component::current_weapon_set>(
                            actor_entity, component::current_weapon_set{actor::weapon_set::SET_2});
                    } else {
                        registry.replace<component::current_weapon_set>(
                            actor_entity, component::current_weapon_set{actor::weapon_set::SET_1});
                    }
                }
            }

            auto side_effect_condition_fn = [&](const configuration::condition_t& condition) {
                return utils::on_finished_casting_conditions_satisfied(
                    condition, actor_entity, skill_configuration, registry);
            };
            utils::apply_side_effects(registry, actor_entity, side_effect_condition_fn);

            utils::enqueue_child_skills(actor_entity,
                                        "Temporary " + skill_configuration.skill_key + " Entity",
                                        skill_configuration.child_skill_keys,
                                        registry);
        }
    });
}

void progress_cooldowns(registry_t& registry) {
    registry.view<component::cooldown_component>().each(
        [&](entity_t entity, component::cooldown_component& cooldown) {
            if (cooldown.duration[0] == 0) {
                auto ammo = registry.try_get<component::ammo>(entity);
                if (ammo) {
                    ammo->current_ammo = ammo->max_ammo;
                    registry.emplace<component::ammo_gained>(entity);
                }
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
                    registry.emplace<component::ammo_gained>(entity);
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
    registry.view<component::skills_actions_component>(entt::exclude<component::has_quickness>)
        .each([&](component::skills_actions_component& casting_skills_component) {
            for (auto& skill_state : casting_skills_component.skills) {
                ++skill_state.pulse_progress[0];
                ++skill_state.strike_progress[0];
            }
        });
    registry.view<component::skills_actions_component, component::has_quickness>().each(
        [&](component::skills_actions_component& casting_skills_component) {
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

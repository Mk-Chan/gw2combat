#include "system.hpp"

#include "gw2combat/utilities/actor_utilities.hpp"
#include "gw2combat/utilities/damage_utilities.hpp"
#include "gw2combat/utilities/entity_utilities.hpp"
#include "gw2combat/utilities/gear_utilities.hpp"

#include "gw2combat/actor/skill_database.hpp"

#include "gw2combat/component/actor/animation.hpp"
#include "gw2combat/component/actor/rotation_component.hpp"
#include "gw2combat/component/actor/team.hpp"
#include "gw2combat/component/damage/effects_pipeline.hpp"
#include "gw2combat/component/damage/strikes_pipeline.hpp"
#include "gw2combat/component/equipment/bundle.hpp"
#include "gw2combat/component/skill/casting_skill.hpp"
#include "gw2combat/component/skill/finished_animation.hpp"

namespace gw2combat::system {

void do_child_skills(const actor::skill_configuration_t& skill_configuration,
                     entity_t entity,
                     registry_t& registry) {
    auto& child_skill_keys = skill_configuration.child_skill_keys;
    utils::enqueue_child_skills(
        entity, skill_configuration.skill_key.name + " Entity", child_skill_keys, registry);
}

void do_weapon_swap(entity_t entity, registry_t& registry) {
    if (registry.any_of<component::bundle_component>(entity)) {
        registry.remove<component::bundle_component>(entity);
        return;
    }
    if (!registry.any_of<component::current_weapon_set>(entity)) {
        throw std::runtime_error("no equipped_weapon_set on entity");
    }
    auto current_set = registry.get<component::current_weapon_set>(entity).set;
    if (current_set == actor::weapon_set::SET_1) {
        registry.replace<component::current_weapon_set>(
            entity, component::current_weapon_set{actor::weapon_set::SET_2});
    } else {
        registry.replace<component::current_weapon_set>(
            entity, component::current_weapon_set{actor::weapon_set::SET_1});
    }
}

void do_strike(const actor::skill_configuration_t& skill_configuration,
               entity_t entity,
               registry_t& registry) {
    if (skill_configuration.hit_on_tick_list.empty() ||
        skill_configuration.hit_on_tick_list[0].empty()) {
        return;
    }

    auto& effective_attributes =
        registry.get<component::effective_attributes>(utils::get_owner(entity, registry));

    double base_outgoing_damage =
        utils::get_weapon_strength(skill_configuration.weapon_type) *
        skill_configuration.damage_coefficient *
        effective_attributes.attribute_value_map[actor::attribute_t::POWER];

    strike::strike_t this_strike = strike::strike_t{
        skill_configuration.skill_key,
        base_outgoing_damage,
        effective_attributes.attribute_value_map[actor::attribute_t::CRITICAL_CHANCE_PCT],
        effective_attributes.attribute_value_map[actor::attribute_t::CRITICAL_DAMAGE_PCT],
        skill_configuration.on_hit_effect_applications,
        skill_configuration.num_targets};

    auto& outgoing_strikes_component =
        registry.get_or_emplace<component::outgoing_strikes_component>(entity);
    outgoing_strikes_component.strikes.emplace_back(this_strike);
}

void do_pulse(const actor::skill_configuration_t& skill_configuration,
              entity_t entity,
              registry_t& registry) {
    auto& outgoing_effects_component =
        registry.get_or_emplace<component::outgoing_effects_component>(entity);
    for (auto& effect_application : skill_configuration.on_pulse_effect_applications) {
        outgoing_effects_component.effect_applications.emplace_back(effect_application);
    }
}

void do_skill(registry_t& registry) {
    registry.view<component::animation>(entt::exclude<component::casting_skill>)
        .each([&](entity_t entity, const component::animation&) {
            registry.emplace<component::casting_skill>(entity);
        });

    registry.view<component::animation, component::casting_skill>().each(
        [&](entity_t entity,
            const component::animation& animation,
            component::casting_skill& casting_skill) {
            int no_quickness_progress_pct =
                animation.duration[0] == 0 ? 100
                                           : animation.progress[0] * 100 / animation.duration[0];
            int quickness_progress_pct = animation.duration[1] == 0
                                             ? 100
                                             : animation.progress[1] * 100 / animation.duration[1];

            int effective_progress_pct = no_quickness_progress_pct + quickness_progress_pct;
            int effective_tick = animation.duration[0] * effective_progress_pct / 100;

            auto skill_configuration =
                registry.get<actor::skill_database>(utils::get_owner(entity, registry))
                    .find_by(animation.skill);
            while (casting_skill.next_pulse_idx <
                       skill_configuration.pulse_on_tick_list[0].size() &&
                   effective_tick >=
                       skill_configuration.pulse_on_tick_list[0][casting_skill.next_pulse_idx]) {
                do_pulse(skill_configuration, entity, registry);
                ++casting_skill.next_pulse_idx;
            }
            while (casting_skill.next_strike_idx < skill_configuration.hit_on_tick_list[0].size() &&
                   effective_tick >=
                       skill_configuration.hit_on_tick_list[0][casting_skill.next_strike_idx]) {
                do_strike(skill_configuration, entity, registry);
                ++casting_skill.next_strike_idx;
            }

            if (effective_progress_pct >= 100) {
                // spdlog::info("[{}]: finished casting skill {}",
                //              utils::get_current_tick(registry),
                //              utils::to_string(skill_configuration.skill_key));
                registry.remove<component::casting_skill>(entity);
            }
        });
    registry.view<component::animation, component::finished_animation>().each(
        [&](entity_t entity,
            const component::animation& animation,
            const component::finished_animation&) {
            auto skill_configuration =
                registry.get<actor::skill_database>(utils::get_owner(entity, registry))
                    .find_by(animation.skill);
            if (skill_configuration.skill_key.name == "Weapon Swap") {
                do_weapon_swap(entity, registry);
            }
            do_child_skills(skill_configuration, entity, registry);
        });
}

}  // namespace gw2combat::system

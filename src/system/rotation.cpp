#include "rotation.hpp"

#include "component/actor/begun_casting_skills.hpp"
#include "component/actor/destroy_after_rotation.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/skills_actions_component.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/encounter/encounter_configuration_component.hpp"
#include "component/equipment/bundle.hpp"
#include "component/lifecycle/destroy_entity.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_conditional_skill_group.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/animation_component.hpp"

#include "utils/actor_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/skill_utils.hpp"

namespace gw2combat::system {

void perform_rotations(registry_t& registry) {
    registry.view<component::rotation_component>(entt::exclude<component::no_more_rotation>)
        .each([&](entity_t entity, component::rotation_component& rotation_component) {
            auto current_tick = utils::get_current_tick(registry);

            if (rotation_component.current_idx >=
                static_cast<int>(rotation_component.rotation.skill_casts.size())) {
                if (rotation_component.repeat) {
                    rotation_component.current_idx = 0;
                    rotation_component.tick_offset = current_tick;
                } else {
                    registry.emplace<component::no_more_rotation>(entity);
                    spdlog::info("[{}] {} has no more rotation",
                                 utils::get_current_tick(registry),
                                 utils::get_entity_name(entity, registry));
                    return;
                }
            }

            auto& next_skill_cast =
                rotation_component.rotation.skill_casts[rotation_component.current_idx];

            // Make sure this skill can only be cast at or after the time specified in the rotation
            // configuration
            if (current_tick < next_skill_cast.cast_time_ms + rotation_component.tick_offset) {
                return;
            }

            auto skill_entity = utils::get_skill_entity(next_skill_cast.skill, entity, registry);

            auto& skill_configuration =
                registry.get<component::is_skill>(skill_entity).skill_configuration;
            bool is_instant_cast_skill = skill_configuration.cast_duration[0] == 0;
            bool is_in_animation = registry.any_of<component::animation_component>(entity);
            if (!is_instant_cast_skill && is_in_animation) {
                return;
            }
            if (!registry
                     .get<component::encounter_configuration_component>(
                         utils::get_singleton_entity())
                     .encounter.require_afk_skills) {
                if (registry.get<component::ammo>(skill_entity).current_ammo <= 0 &&
                    !(skill_configuration.skill_key == "Weapon Swap" &&
                      registry.any_of<component::bundle_component>(entity))) {
                    return;
                }
            }

            auto skill_castability = utils::can_cast_skill(skill_entity, registry);
            if (!skill_castability.can_cast) {
                throw std::runtime_error(fmt::format("[{}] {}: cannot cast skill {}. Reason: {}",
                                                     utils::get_current_tick(registry),
                                                     utils::get_entity_name(entity, registry),
                                                     skill_configuration.skill_key,
                                                     skill_castability.reason));
            }

            auto& skills_actions_component =
                registry.get_or_emplace<component::skills_actions_component>(entity);
            for (auto& skill_to_cancel : skill_configuration.skills_to_cancel) {
                for (auto&& [skill_casting_entity, iter_skills_actions_component] :
                     registry.view<component::skills_actions_component>().each()) {
                    if (utils::get_owner(skill_casting_entity, registry) !=
                        utils::get_owner(entity, registry)) {
                        continue;
                    }
                    for (auto& iter_skill_state : iter_skills_actions_component.skills) {
                        auto& iter_skill_configuration =
                            registry.get<component::is_skill>(iter_skill_state.skill_entity)
                                .skill_configuration;
                        if (iter_skill_configuration.skill_key != skill_to_cancel) {
                            continue;
                        }
                        auto skill_to_cancel_entity = utils::get_skill_entity(
                            skill_to_cancel, skill_casting_entity, registry);
                        auto skill_to_cancel_pos = std::find_if(
                            iter_skills_actions_component.skills.cbegin(),
                            iter_skills_actions_component.skills.cend(),
                            [&](const component::skills_actions_component::skill_state_t&
                                    skill_state) {
                                return skill_state.skill_entity == skill_to_cancel_entity;
                            });
                        if (skill_to_cancel_pos != iter_skills_actions_component.skills.cend()) {
                            spdlog::info(
                                "[{}] {}:{} Canceling skill {} for {}",
                                utils::get_current_tick(registry),
                                utils::get_entity_name(utils::get_owner(entity, registry),
                                                       registry),
                                utils::get_entity_name(entity, registry),
                                skill_to_cancel,
                                utils::get_entity_name(
                                    utils::get_owner(skill_casting_entity, registry), registry));
                            iter_skills_actions_component.skills.erase(skill_to_cancel_pos);
                        }
                    }
                }
            }
            skills_actions_component.skills.emplace_back(
                component::skills_actions_component::skill_state_t{skill_entity});
            auto& begun_casting_skills_component =
                registry.get_or_emplace<component::begun_casting_skills>(entity);
            begun_casting_skills_component.skill_entities.emplace_back(skill_entity);

            rotation_component.current_idx += 1;

            if (is_instant_cast_skill) {
                spdlog::info("[{}] {} casting instant skill {} rotation index {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(entity, registry),
                             utils::to_string(skill_configuration.skill_key),
                             rotation_component.current_idx);
                utils::finish_casting_skill(skill_entity, entity, registry);
            } else {
                registry.emplace<component::animation_component>(
                    entity,
                    component::animation_component{
                        skill_entity, skill_configuration.cast_duration, {0, 0}});
                spdlog::info("[{}] {} casting skill {} rotation index {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(entity, registry),
                             utils::to_string(skill_configuration.skill_key),
                             rotation_component.current_idx);
            }
        });
}

void perform_skills(registry_t& registry) {
    registry.view<component::skills_actions_component>().each(
        [&](entity_t entity, component::skills_actions_component& casting_skills_component) {
            for (auto& casting_skill : casting_skills_component.skills) {
                auto& skill_configuration =
                    registry.get<component::is_skill>(casting_skill.skill_entity)
                        .skill_configuration;

                int pulse_no_quickness_duration =
                    std::max(skill_configuration.pulse_on_tick_list[0].empty()
                                 ? 0
                                 : skill_configuration.pulse_on_tick_list[0].back(),
                             skill_configuration.cast_duration[0]);
                int pulse_quickness_duration =
                    std::max(skill_configuration.pulse_on_tick_list[1].empty()
                                 ? 0
                                 : skill_configuration.pulse_on_tick_list[1].back(),
                             skill_configuration.cast_duration[1]);
                double pulse_no_quickness_progress_pct =
                    pulse_no_quickness_duration == 0.0
                        ? 100.0
                        : (casting_skill.action_progress[0] * 100.0) / pulse_no_quickness_duration;
                double pulse_quickness_progress_pct =
                    pulse_quickness_duration == 0.0
                        ? 100.0
                        : (casting_skill.action_progress[1] * 100.0) / pulse_quickness_duration;
                double pulse_effective_progress_pct =
                    pulse_no_quickness_progress_pct + pulse_quickness_progress_pct;
                int pulse_effective_tick = utils::round_down(pulse_no_quickness_duration *
                                                             pulse_effective_progress_pct / 100.0);
                while (
                    casting_skill.next_pulse_idx <
                        static_cast<int>(skill_configuration.pulse_on_tick_list[0].size()) &&
                    pulse_effective_tick >=
                        skill_configuration.pulse_on_tick_list[0][casting_skill.next_pulse_idx]) {
                    auto& outgoing_effects_component =
                        registry.get_or_emplace<component::outgoing_effects_component>(entity);
                    for (auto& effect_application :
                         skill_configuration.on_pulse_effect_applications) {
                        outgoing_effects_component.effect_applications.emplace_back(
                            component::effect_application_t{
                                .condition = effect_application.condition,
                                .source_skill = skill_configuration.skill_key,
                                .effect = effect_application.effect,
                                .unique_effect = effect_application.unique_effect,
                                .direction = component::effect_application_t::convert_direction(
                                    effect_application.direction),
                                .base_duration_ms = effect_application.base_duration_ms,
                                .num_stacks = effect_application.num_stacks,
                                .num_targets = effect_application.num_targets});
                    }
                    ++casting_skill.next_pulse_idx;
                }

                int strike_no_quickness_duration =
                    std::max(skill_configuration.strike_on_tick_list[0].empty()
                                 ? 0
                                 : skill_configuration.strike_on_tick_list[0].back(),
                             skill_configuration.cast_duration[0]);
                int strike_quickness_duration =
                    std::max(skill_configuration.strike_on_tick_list[1].empty()
                                 ? 0
                                 : skill_configuration.strike_on_tick_list[1].back(),
                             skill_configuration.cast_duration[1]);
                double strike_no_quickness_progress_pct =
                    strike_no_quickness_duration == 0.0
                        ? 100.0
                        : (casting_skill.action_progress[0] * 100.0) / strike_no_quickness_duration;
                double strike_quickness_progress_pct =
                    strike_quickness_duration == 0.0
                        ? 100.0
                        : (casting_skill.action_progress[1] * 100.0) / strike_quickness_duration;
                double strike_effective_progress_pct =
                    strike_no_quickness_progress_pct + strike_quickness_progress_pct;
                int strike_effective_tick = utils::round_down(
                    strike_no_quickness_duration * strike_effective_progress_pct / 100.0);
                while (
                    casting_skill.next_strike_idx <
                        static_cast<int>(skill_configuration.strike_on_tick_list[0].size()) &&
                    strike_effective_tick >=
                        skill_configuration.strike_on_tick_list[0][casting_skill.next_strike_idx]) {
                    auto& outgoing_strikes_component =
                        registry.get_or_emplace<component::outgoing_strikes_component>(entity);
                    auto this_strike = component::strike_t{casting_skill.skill_entity,
                                                           skill_configuration.num_targets};
                    outgoing_strikes_component.strikes.emplace_back(this_strike);
                    ++casting_skill.next_strike_idx;
                }

                int whirl_no_quickness_duration =
                    std::max(skill_configuration.whirl_finisher_on_tick_list[0].empty()
                                 ? 0
                                 : skill_configuration.whirl_finisher_on_tick_list[0].back(),
                             skill_configuration.cast_duration[0]);
                int whirl_quickness_duration =
                    std::max(skill_configuration.whirl_finisher_on_tick_list[1].empty()
                                 ? 0
                                 : skill_configuration.whirl_finisher_on_tick_list[1].back(),
                             skill_configuration.cast_duration[1]);
                double whirl_no_quickness_progress_pct =
                    whirl_no_quickness_duration == 0.0
                        ? 100.0
                        : (casting_skill.action_progress[0] * 100.0) / whirl_no_quickness_duration;
                double whirl_quickness_progress_pct =
                    whirl_quickness_duration == 0.0
                        ? 100.0
                        : (casting_skill.action_progress[1] * 100.0) / whirl_quickness_duration;
                double whirl_effective_progress_pct =
                    whirl_no_quickness_progress_pct + whirl_quickness_progress_pct;
                int whirl_effective_tick = utils::round_down(whirl_no_quickness_duration *
                                                             whirl_effective_progress_pct / 100.0);
                while (casting_skill.next_whirl_idx <
                           static_cast<int>(
                               skill_configuration.whirl_finisher_on_tick_list[0].size()) &&
                       whirl_effective_tick >=
                           skill_configuration
                               .whirl_finisher_on_tick_list[0][casting_skill.next_whirl_idx]) {
                    // NOTE: Eventually maybe want to sort fields cast by us first and then allies
                    actor::combo_field_t our_earliest_combo_field = actor::combo_field_t::INVALID;
                    int most_ticks_among_skills_with_combo_fields = 0;
                    for (auto&& [iter_actor_entity, skill_actions_component] :
                         registry.view<component::skills_actions_component>().each()) {
                        for (auto& iter_skill_state : skill_actions_component.skills) {
                            auto& iter_skill_configuration =
                                registry.get<component::is_skill>(iter_skill_state.skill_entity)
                                    .skill_configuration;
                            if (iter_skill_configuration.combo_field ==
                                actor::combo_field_t::INVALID) {
                                continue;
                            }
                            if ((iter_skill_state.action_progress[0] +
                                 iter_skill_state.action_progress[1]) >
                                most_ticks_among_skills_with_combo_fields) {
                                most_ticks_among_skills_with_combo_fields =
                                    (iter_skill_state.action_progress[0] +
                                     iter_skill_state.action_progress[1]);
                                our_earliest_combo_field = iter_skill_configuration.combo_field;
                            }
                        }
                    }
                    if (our_earliest_combo_field == actor::combo_field_t::FIRE) {
                        utils::enqueue_child_skill("Burning Bolts", entity, registry);
                    }
                    ++casting_skill.next_whirl_idx;
                }

                if (strike_effective_progress_pct >= 100.0 &&
                    pulse_effective_progress_pct >= 100.0 &&
                    whirl_effective_progress_pct >= 100.0) {
                    auto& finished_skills_actions_component =
                        registry.get_or_emplace<component::finished_skills_actions_component>(
                            entity);
                    finished_skills_actions_component.skill_entities.emplace_back(
                        casting_skill.skill_entity);
                    // spdlog::info("[{}] {}: finished skill actions for {}",
                    //              utils::get_current_tick(registry),
                    //              utils::get_entity_name(entity, registry),
                    //              utils::to_string(skill_configuration.skill_key));
                }
            }
        });
}

void cleanup_skill_actions(registry_t& registry) {
    registry
        .view<component::skills_actions_component, component::finished_skills_actions_component>()
        .each([&](entity_t entity,
                  component::skills_actions_component& skills_actions_component,
                  const component::finished_skills_actions_component&
                      finished_skills_actions_component) {
            for (auto skill_entity : finished_skills_actions_component.skill_entities) {
                auto skill_pos = std::find_if(
                    skills_actions_component.skills.cbegin(),
                    skills_actions_component.skills.cend(),
                    [&](const component::skills_actions_component::skill_state_t& skill_state) {
                        return skill_state.skill_entity == skill_entity;
                    });
                if (skill_pos != skills_actions_component.skills.cend()) {
                    skills_actions_component.skills.erase(skill_pos);
                }
            }
            if (skills_actions_component.skills.empty()) {
                registry.remove<component::skills_actions_component>(entity);
            }
            registry.remove<component::finished_skills_actions_component>(entity);
        });
}

void destroy_actors_with_no_rotation(registry_t& registry) {
    registry
        .view<component::destroy_after_rotation, component::no_more_rotation>(
            entt::exclude<component::finished_skills_actions_component,
                          component::skills_actions_component>)
        .each([&](entity_t entity) {
            registry.emplace_or_replace<component::destroy_entity>(entity);
        });
}

}  // namespace gw2combat::system

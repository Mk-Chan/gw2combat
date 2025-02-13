#include "skill.hpp"

#include "component/actor/begun_casting_skills.hpp"
#include "component/actor/skills_actions_component.hpp"
#include "component/actor/skills_ticks_tracker_component.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/encounter/encounter_configuration_component.hpp"
#include "component/skill/is_skill.hpp"

#include "utils/actor_utils.hpp"
#include "utils/condition_utils.hpp"
#include "utils/weapon_utils.hpp"

namespace gw2combat::system {

void perform_skill_ticks(registry_t& registry) {
    auto& encounter =
        registry.get<component::encounter_configuration_component>(utils::get_singleton_entity())
            .encounter;
    registry.view<component::skills_ticks_tracker_component>().each(
        [&](entity_t entity,
            component::skills_ticks_tracker_component& skills_ticks_tracker_component) {
            for (auto& casting_skill_state : skills_ticks_tracker_component.skills) {
                auto& skill_configuration =
                    registry.get<component::is_skill>(casting_skill_state.skill_entity)
                        .skill_configuration;

                if (casting_skill_state.next_skill_tick_idx >=
                    static_cast<int>(skill_configuration.skill_ticks.size())) {
                    auto& destroy_skills_ticks_tracker_component =
                        registry.get_or_emplace<component::destroy_skills_ticks_tracker_component>(
                            entity);
                    destroy_skills_ticks_tracker_component.skill_entities.emplace_back(
                        casting_skill_state.skill_entity);
                    // spdlog::info("[{}] {}: finished skill ticks for {}",
                    //              utils::get_current_tick(registry),
                    //              utils::get_entity_name(entity, registry),
                    //              utils::to_string(skill_configuration.skill_key));
                    continue;
                }

                while (casting_skill_state.next_skill_tick_idx <
                           static_cast<int>(skill_configuration.skill_ticks.size()) &&
                       casting_skill_state.skill_tick_progress >=
                           skill_configuration.skill_ticks[casting_skill_state.next_skill_tick_idx]
                               .on_tick) {
                    auto& this_skill_tick =
                        skill_configuration.skill_ticks[casting_skill_state.next_skill_tick_idx];

                    auto condition_result = utils::independent_conditions_satisfied(
                        this_skill_tick.tick_condition, entity, std::nullopt, registry);
                    if (!condition_result.satisfied) {
                        spdlog::info(
                            "[{}] {}: skill {} on_tick {} condition not satisfied. Reason: {}",
                            utils::get_current_tick(registry),
                            utils::get_entity_name(entity, registry),
                            utils::to_string(skill_configuration.skill_key),
                            this_skill_tick.on_tick,
                            condition_result.reason);
                        ++casting_skill_state.next_skill_tick_idx;
                        continue;
                    }
                    if (this_skill_tick.strike) {
                        auto& outgoing_strikes_component =
                            registry.get_or_emplace<component::outgoing_strikes_component>(entity);
                        actor::weapon_type weapon_type =
                            this_skill_tick.weapon_type == actor::weapon_type::INVALID
                                ? skill_configuration.weapon_type
                                : this_skill_tick.weapon_type;
                        if (!casting_skill_state.weapon_strength_roll_by_group.contains(
                                this_skill_tick.weapon_strength_roll_group)) {
                            casting_skill_state.weapon_strength_roll_by_group
                                [this_skill_tick.weapon_strength_roll_group] =
                                utils::get_weapon_strength(
                                    entity, weapon_type, encounter.weapon_strength_mode, registry);
                        }
                        double weapon_strength_roll =
                            casting_skill_state.weapon_strength_roll_by_group.at(
                                this_skill_tick.weapon_strength_roll_group);
                        auto& tags = this_skill_tick.inherit_tags ? skill_configuration.tags
                                                                  : this_skill_tick.tags;
                        auto this_strike =
                            component::strike_t{casting_skill_state.skill_entity,
                                                this_skill_tick.num_targets,
                                                this_skill_tick.flat_damage,
                                                weapon_strength_roll,
                                                this_skill_tick.damage_coefficient,
                                                this_skill_tick.can_critical_strike,
                                                this_skill_tick.on_strike_effect_applications,
                                                tags};
                        outgoing_strikes_component.strikes.emplace_back(this_strike);
                    }
                    if (this_skill_tick.pulse) {
                        auto& outgoing_effects_component =
                            registry.get_or_emplace<component::outgoing_effects_component>(entity);
                        std::transform(
                            this_skill_tick.on_pulse_effect_applications.begin(),
                            this_skill_tick.on_pulse_effect_applications.end(),
                            std::back_inserter(outgoing_effects_component.effect_applications),
                            [&](const configuration::effect_application_t& effect_application) {
                                return component::effect_application_t{
                                    .condition = effect_application.condition,
                                    .source_skill = skill_configuration.skill_key,
                                    .effect = effect_application.effect,
                                    .unique_effect = effect_application.unique_effect,
                                    .direction = component::effect_application_t::convert_direction(
                                        effect_application.direction),
                                    .base_duration_ms = effect_application.base_duration_ms,
                                    .num_stacks = effect_application.num_stacks,
                                    .num_targets = effect_application.num_targets};
                            });
                    }

                    if (this_skill_tick.whirl_finisher) {
                        // NOTE: Eventually maybe want to sort fields cast by us first and then
                        // allies
                        actor::combo_field_t our_earliest_combo_field =
                            actor::combo_field_t::INVALID;
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
                    }

                    for (auto& skill_to_put_on_cooldown :
                         this_skill_tick.skills_to_put_on_cooldown) {
                        auto skill_to_put_on_cooldown_entity =
                            utils::get_skill_entity(skill_to_put_on_cooldown, entity, registry);
                        utils::put_skill_on_cooldown(
                            skill_to_put_on_cooldown_entity, registry, true);
                    }
                    for (auto& skill_to_cancel : this_skill_tick.skills_to_cancel) {
                        for (auto&& [skill_casting_entity, iter_skills_ticks_tracker_component] :
                             registry.view<component::skills_ticks_tracker_component>().each()) {
                            if (utils::get_owner(skill_casting_entity, registry) !=
                                utils::get_owner(entity, registry)) {
                                continue;
                            }
                            for (auto& iter_skill_state :
                                 iter_skills_ticks_tracker_component.skills) {
                                auto& iter_skill_configuration =
                                    registry.get<component::is_skill>(iter_skill_state.skill_entity)
                                        .skill_configuration;
                                if (iter_skill_configuration.skill_key != skill_to_cancel) {
                                    continue;
                                }
                                auto skill_to_cancel_entity = utils::get_skill_entity(
                                    skill_to_cancel, skill_casting_entity, registry);
                                auto skill_to_cancel_pos = std::find_if(
                                    iter_skills_ticks_tracker_component.skills.cbegin(),
                                    iter_skills_ticks_tracker_component.skills.cend(),
                                    [&](const component::skills_ticks_tracker_component::
                                            skill_state_t& skill_state) {
                                        return skill_state.skill_entity == skill_to_cancel_entity;
                                    });
                                if (skill_to_cancel_pos !=
                                    iter_skills_ticks_tracker_component.skills.cend()) {
                                    spdlog::info(
                                        "[{}] {}:{} Canceling skill ticks in skill {} for {}",
                                        utils::get_current_tick(registry),
                                        utils::get_entity_name(utils::get_owner(entity, registry),
                                                               registry),
                                        utils::get_entity_name(entity, registry),
                                        skill_to_cancel,
                                        utils::get_entity_name(
                                            utils::get_owner(skill_casting_entity, registry),
                                            registry));
                                    iter_skills_ticks_tracker_component.skills.erase(
                                        skill_to_cancel_pos);
                                }
                            }
                        }
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
                                if (skill_to_cancel_pos !=
                                    iter_skills_actions_component.skills.cend()) {
                                    spdlog::info(
                                        "[{}] {}:{} Canceling skill actions in skill {} for {}",
                                        utils::get_current_tick(registry),
                                        utils::get_entity_name(utils::get_owner(entity, registry),
                                                               registry),
                                        utils::get_entity_name(entity, registry),
                                        skill_to_cancel,
                                        utils::get_entity_name(
                                            utils::get_owner(skill_casting_entity, registry),
                                            registry));
                                    iter_skills_actions_component.skills.erase(skill_to_cancel_pos);
                                }
                            }
                        }
                    }

                    utils::enqueue_child_skills(
                        entity,
                        "Temporary " + skill_configuration.skill_key + " Entity",
                        this_skill_tick.child_skill_keys,
                        registry);

                    ++casting_skill_state.next_skill_tick_idx;
                }
                if (casting_skill_state.next_skill_tick_idx >=
                    static_cast<int>(skill_configuration.skill_ticks.size())) {
                    auto& destroy_skills_ticks_tracker_component =
                        registry.get_or_emplace<component::destroy_skills_ticks_tracker_component>(
                            entity);
                    destroy_skills_ticks_tracker_component.skill_entities.emplace_back(
                        casting_skill_state.skill_entity);
                    spdlog::info("[{}] {}: finished skill ticks for {}",
                                 utils::get_current_tick(registry),
                                 utils::get_entity_name(entity, registry),
                                 utils::to_string(skill_configuration.skill_key));
                }
            }
        });
}

void perform_skills(registry_t& registry) {
    registry.view<component::skills_actions_component>().each(
        [&](entity_t entity, component::skills_actions_component& skills_actions_component) {
            for (auto& casting_skill : skills_actions_component.skills) {
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
                    auto this_strike =
                        component::strike_t{casting_skill.skill_entity,
                                            skill_configuration.num_targets,
                                            skill_configuration.flat_damage,
                                            casting_skill.weapon_strength_roll,
                                            skill_configuration.damage_coefficient,
                                            skill_configuration.can_critical_strike,
                                            skill_configuration.on_strike_effect_applications,
                                            skill_configuration.tags};
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

void cleanup_skill_ticks_tracker(registry_t& registry) {
    registry
        .view<component::skills_ticks_tracker_component,
              component::destroy_skills_ticks_tracker_component>()
        .each([&](entity_t entity,
                  component::skills_ticks_tracker_component& skills_ticks_tracker_component,
                  const component::destroy_skills_ticks_tracker_component&
                      destroy_skills_ticks_tracker_component) {
            for (auto skill_entity : destroy_skills_ticks_tracker_component.skill_entities) {
                auto skill_pos = std::find_if(
                    skills_ticks_tracker_component.skills.cbegin(),
                    skills_ticks_tracker_component.skills.cend(),
                    [&](const component::skills_ticks_tracker_component::skill_state_t&
                            skill_state) { return skill_state.skill_entity == skill_entity; });
                if (skill_pos != skills_ticks_tracker_component.skills.cend()) {
                    skills_ticks_tracker_component.skills.erase(skill_pos);
                }
            }
            if (skills_ticks_tracker_component.skills.empty()) {
                registry.remove<component::skills_ticks_tracker_component>(entity);
            }
            registry.remove<component::destroy_skills_ticks_tracker_component>(entity);
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

}  // namespace gw2combat::system

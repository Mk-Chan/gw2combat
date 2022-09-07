#include "gw2combat/common.hpp"

#include "gw2combat/actor/skill_database.hpp"

#include "gw2combat/component/actor/traits_component.hpp"
#include "gw2combat/component/equipment/weapons.hpp"
#include "gw2combat/component/filters.hpp"
#include "gw2combat/component/owner_actor.hpp"
#include "gw2combat/component/skill_modifiers_component.hpp"

namespace gw2combat {

inline bool filters_satisfied(const component::filters_t& filters,
                              entity_t owner_entity,
                              const actor::skill_configuration_t& skill_configuration,
                              registry_t& registry) {
    if (!registry.any_of<component::bundle_component>(owner_entity) &&
        (filters.weapon_type || filters.weapon_position)) {
        bool is_satisfied =
            skill_configuration.weapon_type == *filters.weapon_type &&
            (skill_configuration.weapon_position == actor::weapon_position::UNIVERSAL ||
             skill_configuration.weapon_position == *filters.weapon_position);
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.source_trait) {
        if (!registry.any_of<component::traits_component>(owner_entity)) {
            return false;
        }
        auto& trait_entities =
            registry.get<component::traits_component>(owner_entity).trait_entities;
        bool is_satisfied = std::any_of(trait_entities.begin(),
                                        trait_entities.end(),
                                        [&](const component::trait_entity& trait_entity) {
                                            return trait_entity.trait == *filters.source_trait;
                                        });
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.source_skill) {
        bool is_satisfied = filters.source_skill == skill_configuration.skill_key;
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.source_skill_tag) {
        bool is_satisfied = skill_configuration.tags.contains(*filters.source_skill_tag);
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.target_actor_effect || filters.source_actor_effect || filters.weapon_sigil ||
        filters.rune) {
        return false;
    }
    return true;
}

void build_skills(registry_t& registry) {
    registry.view<component::owner_actor, component::skill_modifiers_component>().each(
        [&](const component::owner_actor& owner_actor,
            const component::skill_modifiers_component& skill_modifiers_component) {
            auto& skill_database = registry.get<actor::skill_database>(owner_actor.entity);
            for (auto& skill_modifiers : skill_modifiers_component.skill_modifiers) {
                for (auto& skill_configuration : skill_database.skill_configurations) {
                    if (filters_satisfied(skill_modifiers.filters,
                                          owner_actor.entity,
                                          skill_configuration,
                                          registry)) {
                        skill_configuration.cooldown[0] -=
                            (int)(skill_modifiers.cooldown_reduction_pct *
                                  (double)skill_configuration.cooldown[0] / 100.0);
                        skill_configuration.cooldown[1] -=
                            (int)(skill_modifiers.cooldown_reduction_pct *
                                  (double)skill_configuration.cooldown[1] / 100.0);

                        if (!skill_modifiers.duration_increase_ms.empty()) {
                            double strikes_per_tick =
                                (double)skill_configuration.strike_on_tick_list[0].size() /
                                (double)skill_configuration.cast_duration[0];
                            double pulses_per_tick =
                                (double)skill_configuration.pulse_on_tick_list[0].size() /
                                (double)skill_configuration.cast_duration[0];

                            skill_configuration.cast_duration[0] +=
                                skill_modifiers.duration_increase_ms[0];
                            skill_configuration.cast_duration[1] +=
                                skill_modifiers.duration_increase_ms[1];

                            if (skill_configuration.strike_on_tick_list[0].size() > 1) {
                                int additional_strikes =
                                    (int)((double)skill_configuration.cast_duration[0] *
                                          strikes_per_tick) -
                                    (int)skill_configuration.strike_on_tick_list[0].size();
                                int no_quickness_final_strike_ms =
                                    skill_configuration.strike_on_tick_list
                                        [0][skill_configuration.strike_on_tick_list[0].size() - 1];
                                int no_quickness_strike_interval =
                                    no_quickness_final_strike_ms -
                                    skill_configuration.strike_on_tick_list
                                        [0][skill_configuration.strike_on_tick_list[0].size() - 2];
                                int quickness_final_strike_ms =
                                    skill_configuration.strike_on_tick_list
                                        [1][skill_configuration.strike_on_tick_list[1].size() - 1];
                                int quickness_strike_interval =
                                    quickness_final_strike_ms -
                                    skill_configuration.strike_on_tick_list
                                        [1][skill_configuration.strike_on_tick_list[1].size() - 2];
                                for (int i = 0; i < additional_strikes; ++i) {
                                    skill_configuration.strike_on_tick_list[0].emplace_back(
                                        no_quickness_final_strike_ms +
                                        (i + 1) * no_quickness_strike_interval);
                                    skill_configuration.strike_on_tick_list[1].emplace_back(
                                        quickness_final_strike_ms +
                                        (i + 1) * quickness_strike_interval);
                                }
                            }
                            if (skill_configuration.pulse_on_tick_list[0].size() > 1) {
                                int additional_pulses =
                                    (int)((double)skill_configuration.cast_duration[0] *
                                          pulses_per_tick) -
                                    (int)skill_configuration.pulse_on_tick_list[0].size();
                                int no_quickness_final_pulse_ms =
                                    skill_configuration.pulse_on_tick_list
                                        [0][skill_configuration.pulse_on_tick_list[0].size() - 1];
                                int no_quickness_pulse_interval =
                                    no_quickness_final_pulse_ms -
                                    skill_configuration.pulse_on_tick_list
                                        [0][skill_configuration.pulse_on_tick_list[0].size() - 2];
                                int quickness_final_pulse_ms =
                                    skill_configuration.pulse_on_tick_list
                                        [1][skill_configuration.pulse_on_tick_list[1].size() - 1];
                                int quickness_pulse_interval =
                                    quickness_final_pulse_ms -
                                    skill_configuration.pulse_on_tick_list
                                        [1][skill_configuration.pulse_on_tick_list[1].size() - 2];
                                for (int i = 0; i < additional_pulses; ++i) {
                                    skill_configuration.pulse_on_tick_list[0].emplace_back(
                                        no_quickness_final_pulse_ms +
                                        (i + 1) * no_quickness_pulse_interval);
                                    skill_configuration.pulse_on_tick_list[1].emplace_back(
                                        quickness_final_pulse_ms +
                                        (i + 1) * quickness_pulse_interval);
                                }
                            }
                        }

                        std::copy(
                            skill_modifiers.on_strike_effect_applications.begin(),
                            skill_modifiers.on_strike_effect_applications.end(),
                            std::back_inserter(skill_configuration.on_strike_effect_applications));
                    }
                }
            }
        });
}

}  // namespace gw2combat

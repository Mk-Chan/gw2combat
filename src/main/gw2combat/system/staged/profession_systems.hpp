#ifndef GW2COMBAT_SYSTEM_PROFESSION_PROFESSION_SYSTEMS_HPP
#define GW2COMBAT_SYSTEM_PROFESSION_PROFESSION_SYSTEMS_HPP

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/damage/source_entity.hpp"
#include "gw2combat/component/profession_components.hpp"

namespace gw2combat::system {

template <combat_stage stage>
static inline void virtue_of_justice(registry_t& registry, tick_t current_tick) {
    if constexpr (stage == combat_stage::BEFORE_OUTGOING_STRIKE_BUFFERING) {
        registry.template view<component::virtue_of_justice>().template each(
            [&](component::virtue_of_justice& virtue_of_justice) {
                virtue_of_justice.cooldown_progress =
                    std::min(virtue_of_justice.cooldown_progress + 1, virtue_of_justice.cooldown);
            });

        registry
            .template view<component::finished_normal_cast_skill, component::virtue_of_justice>()
            .each([&](entity_t entity,
                      const component::finished_normal_cast_skill& finished_normal_casting_skill,
                      component::virtue_of_justice& virtue_of_justice) {
                if (utils::has_trait(trait_type::LOREMASTER, entity, registry)) {
                    return;
                }

                if (finished_normal_casting_skill.skill.name == "Virtue of Justice" ||
                    finished_normal_casting_skill.skill.name == "Spear of Justice" ||
                    finished_normal_casting_skill.skill.name == "Tome of Justice") {
                    // FIXME: Rework alacrity and quickness to be consistent across losing and
                    //        regaining in the middle of skills
                    bool has_alacrity =
                        utils::has_effect(effects::effect_type::ALACRITY, entity, registry);

                    virtue_of_justice.cooldown =
                        finished_normal_casting_skill.skill.cooldown[has_alacrity];
                    virtue_of_justice.cooldown_progress = 0;
                    ++virtue_of_justice.num_unaccounted_hits;
                    spdlog::info(
                        "tick: {}, entity: {}, virtue of justice passive went on cooldown for {}",
                        current_tick,
                        utils::get_entity_name(entity, registry),
                        finished_normal_casting_skill.skill.cooldown[has_alacrity]);
                }
            });
    } else if (stage == combat_stage::BEFORE_INCOMING_STRIKE_DAMAGE_CALCULATION) {
        registry.view<component::incoming_strike_damage>().each(
            [&](component::incoming_strike_damage& incoming_strike_damage) {
                for (auto& strike : incoming_strike_damage.strikes) {
                    entity_t source_entity = utils::get_source_entity(strike.source, registry);
                    if (utils::has_effect(
                            effects::effect_type::VIRTUE_OF_JUSTICE, source_entity, registry)) {
                        strike.skill.on_hit_effect_applications.template emplace_back(
                            skills::effect_application{effects::effect_type::BURNING,
                                                       skills::applied_effect_direction::OUTGOING,
                                                       1,
                                                       4'000});
                        registry.template get<component::virtue_of_justice>(source_entity)
                            .num_unaccounted_hits = 0;
                        utils::remove_all_effect_stacks(
                            effects::effect_type::VIRTUE_OF_JUSTICE, source_entity, registry);
                    }
                }
            });
    } else if constexpr (stage == combat_stage::AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION) {
        registry.view<component::incoming_strike_damage>().each(
            [&](const component::incoming_strike_damage& incoming_strike_damage) {
                for (const auto& strike : incoming_strike_damage.strikes) {
                    entity_t source_entity = utils::get_source_entity(strike.source, registry);
                    auto virtue_of_justice_ptr =
                        registry.try_get<component::virtue_of_justice>(source_entity);
                    if (virtue_of_justice_ptr && virtue_of_justice_ptr->cooldown_progress >=
                                                     virtue_of_justice_ptr->cooldown) {
                        ++virtue_of_justice_ptr->num_unaccounted_hits;
                    }
                }
            });

        registry.view<component::effective_attributes, component::virtue_of_justice>().each(
            [&](entity_t entity,
                const component::effective_attributes& effective_attributes,
                component::virtue_of_justice& virtue_of_justice) {
                unsigned int burning_stacks_to_apply =
                    virtue_of_justice.num_unaccounted_hits /
                    virtue_of_justice.number_of_ticks_for_burning_application;
                if (burning_stacks_to_apply > 0) {
                    virtue_of_justice.num_unaccounted_hits %=
                        virtue_of_justice.number_of_ticks_for_burning_application;

                    double duration_modifier =
                        utils::has_trait(trait_type::AMPLIFIED_WRATH, entity, registry) ? 1.2 : 1.0;
                    auto effective_duration =
                        utils::get_effective_effect_duration((tick_t)(2'000.0 * duration_modifier),
                                                             effects::effect_type::BURNING,
                                                             effective_attributes);
                    auto& outgoing_condition_application =
                        registry.get_or_emplace<component::outgoing_condition_application>(entity);
                    outgoing_condition_application.effect_applications.template emplace_back(
                        effects::effect_application{effects::effect_type::BURNING,
                                                    entity,
                                                    effective_duration,
                                                    burning_stacks_to_apply});

                    spdlog::info("tick: {}, entity: {}, outgoing burning from virtue_of_justice",
                                 current_tick,
                                 utils::get_entity_name(entity, registry));
                }
            });
    }
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_PROFESSION_PROFESSION_SYSTEMS_HPP

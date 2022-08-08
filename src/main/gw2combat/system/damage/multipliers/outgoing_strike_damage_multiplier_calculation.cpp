#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/combat_stats.hpp"
#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/source_entity.hpp"
#include "gw2combat/component/gear/runes.hpp"

namespace gw2combat::system {

void outgoing_strike_damage_multiplier_calculation(registry_t& registry, tick_t) {
    registry.view<component::effective_attributes, component::combat_stats>().each(
        [&](entity_t entity,
            component::effective_attributes& effective_attributes,
            const component::combat_stats& combat_stats) {
            bool has_scholar_rune = utils::has_rune(rune_type::SCHOLAR, entity, registry);
            bool has_force_sigil = utils::has_sigil_equipped(weapon_sigil::FORCE, entity, registry);
            bool has_impact_sigil =
                utils::has_sigil_equipped(weapon_sigil::IMPACT, entity, registry);

            auto traits_component_ptr = registry.try_get<component::traits_component>(entity);
            bool retribution_is_traited =
                utils::has_trait(trait_type::RETRIBUTION, traits_component_ptr);
            bool unscathed_contender_is_traited =
                utils::has_trait(trait_type::UNSCATHED_CONTENDER, traits_component_ptr);
            bool fiery_wrath_is_traited =
                utils::has_trait(trait_type::FIERY_WRATH, traits_component_ptr);
            bool symbolic_exposure_is_traited =
                utils::has_trait(trait_type::SYMBOLIC_EXPOSURE, traits_component_ptr);
            bool inspired_virtue_is_traited =
                utils::has_trait(trait_type::INSPIRED_VIRTUE, traits_component_ptr);

            auto effects_component_ptr = registry.try_get<component::effects_component>(entity);
            bool has_inspiring_virtue_effect =
                utils::has_effect(effects::effect_type::INSPIRING_VIRTUE, effects_component_ptr);
            bool has_aegis = utils::has_effect(effects::effect_type::AEGIS, effects_component_ptr);
            bool has_resolution =
                utils::has_effect(effects::effect_type::RESOLUTION, effects_component_ptr);

            size_t symbolic_avenger_stacks = utils::get_num_stacks_of_effect(
                effects::effect_type::SYMBOLIC_AVENGER, effects_component_ptr);
            double symbolic_avenger_addend = ((double)symbolic_avenger_stacks) * 0.02;

            double scholar_rune_multiplier = 1.0;
            bool is_above_90pct_health =
                combat_stats.health > (effective_attributes.max_health * 0.90);
            scholar_rune_multiplier += has_scholar_rune * is_above_90pct_health * 0.05;

            double force_sigil_addend = has_force_sigil * 0.05;
            double impact_sigil_addend = has_impact_sigil * 0.03;
            double retribution_addend = retribution_is_traited * has_resolution * 0.1;
            double unscathed_contender_addend = unscathed_contender_is_traited * has_aegis * 0.2;
            double inspiring_virtue_addend = has_inspiring_virtue_effect * 0.1;

            double fiery_wrath_multiplier = 1.0;
            double symbolic_exposure_multiplier = 1.0;
            auto targeting_ptr = registry.try_get<component::targeting>(entity);
            if (targeting_ptr) {
                auto target_effects_component_ptr =
                    registry.try_get<component::effects_component>(targeting_ptr->entity);

                bool target_is_burning =
                    utils::has_effect(effects::effect_type::BURNING, target_effects_component_ptr);
                // FIXME: This is a bit of a hack maybe. const the effective attrs if you move this.
                //        The right way is to roll crit for each strike and apply this at a later
                //        stage
                if (target_is_burning &&
                    utils::has_trait(trait_type::RADIANT_POWER, entity, registry)) {
                    effective_attributes.critical_chance_pct += 10;
                }

                fiery_wrath_multiplier += fiery_wrath_is_traited * target_is_burning * 0.07;

                bool target_is_vulnerable = utils::has_effect(effects::effect_type::VULNERABILITY,
                                                              target_effects_component_ptr);
                symbolic_exposure_multiplier +=
                    symbolic_exposure_is_traited * target_is_vulnerable * 0.05;

                // TODO: Add logic to check stunned target for impact sigil's second part
            }

            double inspired_virtue_multiplier = 1.0;
            if (effects_component_ptr != nullptr) {
                size_t boon_count = 0;
                for (auto boon : utils::BOONS) {
                    boon_count += utils::has_effect(boon, effects_component_ptr);
                }
                inspired_virtue_multiplier +=
                    (inspired_virtue_is_traited * (double)boon_count * 0.01);
            }

            double addends_multiplier = (1.0 + (force_sigil_addend + impact_sigil_addend +
                                                retribution_addend + unscathed_contender_addend +
                                                symbolic_avenger_addend + inspiring_virtue_addend));

            double final_multiplier = addends_multiplier * scholar_rune_multiplier *
                                      inspired_virtue_multiplier * fiery_wrath_multiplier *
                                      symbolic_exposure_multiplier * effective_attributes.power;
            registry.emplace<component::outgoing_strike_damage_multiplier>(
                entity, component::outgoing_strike_damage_multiplier{final_multiplier});
        });
    registry.view<component::source_entity>().each(
        [&](entity_t entity, const component::source_entity& outgoing_damage_source) {
            auto source_entity = utils::get_source_entity(outgoing_damage_source.entity, registry);
            auto& source_outgoing_strike_damage_multiplier =
                registry.get<component::outgoing_strike_damage_multiplier>(source_entity);
            registry.emplace<component::outgoing_strike_damage_multiplier>(
                entity, source_outgoing_strike_damage_multiplier);
        });
}

}  // namespace gw2combat::system

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/combat_stats.hpp"
#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/source_entity.hpp"
#include "gw2combat/component/effect_components.hpp"
#include "gw2combat/component/gear/runes.hpp"

namespace gw2combat::system {

void outgoing_strike_damage_multiplier_calculation(registry_t& registry, tick_t) {
    registry.view<component::effective_attributes, component::combat_stats>().each(
        [&](entity_t entity,
            const component::effective_attributes& effective_attributes,
            const component::combat_stats& combat_stats) {
            bool has_scholar_rune = utils::has_rune(rune_type::SCHOLAR, entity, registry);
            bool has_force_sigil = utils::has_sigil(weapon_sigil::FORCE, entity, registry);
            bool has_impact_sigil = utils::has_sigil(weapon_sigil::IMPACT, entity, registry);
            bool retribution_is_traited =
                utils::has_trait(trait_type::RETRIBUTION, entity, registry);
            bool unscathed_contender_is_traited =
                utils::has_trait(trait_type::UNSCATHED_CONTENDER, entity, registry);
            bool fiery_wrath_is_traited =
                utils::has_trait(trait_type::FIERY_WRATH, entity, registry);
            bool symbolic_exposure_is_traited =
                utils::has_trait(trait_type::SYMBOLIC_POWER, entity, registry);
            bool inspired_virtue_is_traited =
                utils::has_trait(trait_type::INSPIRING_VIRTUE, entity, registry);

            bool has_inspiring_virtue_effect =
                registry.any_of<component::inspiring_virtue_effect>(entity);

            bool has_aegis = registry.any_of<component::aegis>(entity);
            bool has_might = registry.any_of<component::might>(entity);
            bool has_fury = registry.any_of<component::fury>(entity);
            bool has_quickness = registry.any_of<component::quickness>(entity);
            bool has_alacrity = registry.any_of<component::alacrity>(entity);
            bool has_resolution = registry.any_of<component::resolution>(entity);

            double symbolic_avenger_addend = 0.0;
            auto symbolic_avenger_ptr =
                registry.try_get<component::symbolic_avenger_effect>(entity);
            if (symbolic_avenger_ptr) {
                symbolic_avenger_addend +=
                    std::min((double)symbolic_avenger_ptr->effect_old.num_stacks(), 5.0) * 0.02;
            }

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
                bool target_is_burning = registry.any_of<component::burning>(targeting_ptr->entity);
                fiery_wrath_multiplier += fiery_wrath_is_traited * target_is_burning * 0.07;

                bool target_is_vulnerable =
                    registry.any_of<component::vulnerability>(targeting_ptr->entity);
                symbolic_exposure_multiplier +=
                    symbolic_exposure_is_traited * target_is_vulnerable * 0.05;

                // TODO: Add logic to check stunned target for impact sigil's second part
            }

            unsigned int boon_count =
                has_aegis + has_might + has_fury + has_quickness + has_alacrity + has_resolution;
            double inspired_virtue_multiplier =
                1.0 + (inspired_virtue_is_traited * boon_count * 0.01);

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

#include "gw2combat/system/system.hpp"

#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

#include "gw2combat/component/boon/aegis.hpp"
#include "gw2combat/component/boon/alacrity.hpp"
#include "gw2combat/component/boon/fury.hpp"
#include "gw2combat/component/boon/might.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/boon/resolution.hpp"
#include "gw2combat/component/character/combat_stats.hpp"
#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/condition/vulnerability.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/outgoing_damage_source.hpp"
#include "gw2combat/component/gear/rune/rune_scholar.hpp"
#include "gw2combat/component/gear/sigil/sigil_force.hpp"
#include "gw2combat/component/gear/sigil/sigil_impact.hpp"
#include "gw2combat/component/traits/guardian/fiery_wrath.hpp"
#include "gw2combat/component/traits/guardian/inspired_virtue.hpp"
#include "gw2combat/component/traits/guardian/retribution.hpp"
#include "gw2combat/component/traits/guardian/symbolic_avenger.hpp"
#include "gw2combat/component/traits/guardian/symbolic_exposure.hpp"
#include "gw2combat/component/traits/guardian/unscathed_contender.hpp"

namespace gw2combat::system {

void outgoing_strike_damage_multiplier_calculation(context& ctx) {
    ctx.registry.view<component::effective_attributes, component::combat_stats>().each(
        [&](const entt::entity entity,
            const component::effective_attributes& effective_attributes,
            const component::combat_stats& combat_stats) {
            bool has_scholar_rune = ctx.registry.any_of<component::rune_scholar>(entity);
            bool has_force_sigil = ctx.registry.any_of<component::sigil_force>(entity);
            bool has_impact_sigil = ctx.registry.any_of<component::sigil_impact>(entity);
            bool retribution_is_traited = ctx.registry.any_of<component::retribution>(entity);
            bool unscathed_contender_is_traited =
                ctx.registry.any_of<component::unscathed_contender>(entity);
            bool fiery_wrath_is_traited = ctx.registry.any_of<component::fiery_wrath>(entity);
            bool symbolic_exposure_is_traited =
                ctx.registry.any_of<component::symbolic_exposure>(entity);
            bool inspired_virtue_is_traited =
                ctx.registry.any_of<component::inspired_virtue>(entity);

            bool has_aegis = ctx.registry.any_of<component::aegis>(entity);
            bool has_might = ctx.registry.any_of<component::might>(entity);
            bool has_fury = ctx.registry.any_of<component::fury>(entity);
            bool has_quickness = ctx.registry.any_of<component::quickness>(entity);
            bool has_alacrity = ctx.registry.any_of<component::alacrity>(entity);
            bool has_resolution = ctx.registry.any_of<component::resolution>(entity);

            double symbolic_avenger_addend = 0.0;
            auto symbolic_avenger_ptr = ctx.registry.try_get<component::symbolic_avenger>(entity);
            if (symbolic_avenger_ptr) {
                symbolic_avenger_addend +=
                    std::min((double)symbolic_avenger_ptr->stacks.size(), 5.0) *
                    component::symbolic_avenger::strike_damage_increase_per_stack;
            }

            double scholar_rune_multiplier = 1.0;
            bool is_above_90pct_health =
                combat_stats.health > (effective_attributes.max_health * 0.90);
            scholar_rune_multiplier +=
                has_scholar_rune * is_above_90pct_health *
                component::rune_scholar::strike_damage_increase_if_above_90pct_health;

            double force_sigil_addend =
                has_force_sigil * component::sigil_force::strike_damage_increase;

            double impact_sigil_addend =
                has_impact_sigil * component::sigil_impact::strike_damage_increase;

            double retribution_addend =
                retribution_is_traited * has_resolution *
                component::retribution::strike_damage_increase_if_has_resolution;

            double unscathed_contender_addend =
                unscathed_contender_is_traited * has_aegis *
                component::unscathed_contender::strike_damage_increase_if_has_aegis;

            double fiery_wrath_multiplier = 1.0;
            double symbolic_exposure_multiplier = 1.0;
            auto targeting_ptr = ctx.registry.try_get<component::targeting>(entity);
            if (targeting_ptr) {
                bool target_is_burning =
                    ctx.registry.any_of<component::burning>(targeting_ptr->entity);
                fiery_wrath_multiplier +=
                    fiery_wrath_is_traited * target_is_burning *
                    component::fiery_wrath::strike_damage_increase_if_target_burning;

                bool target_is_vulnerable =
                    ctx.registry.any_of<component::vulnerability>(targeting_ptr->entity);
                symbolic_exposure_multiplier +=
                    symbolic_exposure_is_traited * target_is_vulnerable *
                    component::symbolic_exposure::strike_damage_increase_if_target_vulnerable;

                // TODO: Add logic to check stunned target for impact sigil's second part
            }

            unsigned int boon_count =
                has_aegis + has_might + has_fury + has_quickness + has_alacrity + has_resolution;
            double inspired_virtue_multiplier =
                1.0 + (inspired_virtue_is_traited * boon_count *
                       component::inspired_virtue::strike_damage_increase_per_boon);

            double addends_multiplier =
                (1.0 + (force_sigil_addend + impact_sigil_addend + retribution_addend +
                        unscathed_contender_addend + symbolic_avenger_addend));

            // TODO: Move this out into another thing
            double critical_hit_multiplier =
                (1.0 + (std::min(effective_attributes.critical_chance_pct, 100.0) / 100.0) *
                           (effective_attributes.critical_damage_pct / 100.0 - 1.0));

            double final_multiplier = addends_multiplier * scholar_rune_multiplier *
                                      inspired_virtue_multiplier * fiery_wrath_multiplier *
                                      symbolic_exposure_multiplier * critical_hit_multiplier *
                                      effective_attributes.power;
            ctx.registry.emplace<component::outgoing_strike_damage_multiplier>(
                entity, component::outgoing_strike_damage_multiplier{final_multiplier});
        });
    ctx.registry.view<component::outgoing_damage_source>().each(
        [&](const entt::entity entity,
            const component::outgoing_damage_source& outgoing_damage_source) {
            auto source_entity = outgoing_damage_source.source;
            auto& source_outgoing_strike_damage_multiplier =
                ctx.registry.get<component::outgoing_strike_damage_multiplier>(source_entity);
            ctx.registry.emplace<component::outgoing_strike_damage_multiplier>(
                entity, source_outgoing_strike_damage_multiplier);
        });
}

}  // namespace gw2combat::system

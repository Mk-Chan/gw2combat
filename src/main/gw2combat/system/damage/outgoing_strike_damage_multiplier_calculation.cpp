#include "gw2combat/system/system.hpp"

#include <entt/entt.hpp>

#include "gw2combat/component/boon/aegis.hpp"
#include "gw2combat/component/boon/fury.hpp"
#include "gw2combat/component/boon/might.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/boon/resolution.hpp"
#include "gw2combat/component/combat_stats.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/condition/vulnerability.hpp"
#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/gear/rune/rune_scholar.hpp"
#include "gw2combat/component/gear/sigil/sigil_force.hpp"
#include "gw2combat/component/gear/sigil/sigil_impact.hpp"
#include "gw2combat/component/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/profession/guardian/trait/fiery_wrath.hpp"
#include "gw2combat/component/profession/guardian/trait/inspired_virtue.hpp"
#include "gw2combat/component/profession/guardian/trait/retribution.hpp"
#include "gw2combat/component/profession/guardian/trait/symbolic_exposure.hpp"
#include "gw2combat/component/profession/guardian/trait/unscathed_contender.hpp"
#include "gw2combat/component/targeting.hpp"

namespace gw2combat::system {

void outgoing_strike_damage_multiplier_calculation(context& ctx) {
    ctx.registry.view<component::effective_attributes, component::combat_stats>().each(
        [&](const entt::entity entity,
            const component::effective_attributes& effective_attributes,
            const component::combat_stats& combat_stats) {
            double scholar_rune_multiplier = 1.0;
            bool has_scholar_rune = ctx.registry.any_of<component::rune_scholar>(entity);
            bool is_above_90pct_health =
                combat_stats.health > (effective_attributes.max_health * 0.90);
            scholar_rune_multiplier +=
                has_scholar_rune * is_above_90pct_health *
                component::rune_scholar::strike_damage_increase_if_above_90pct_health;

            bool has_force_sigil = ctx.registry.any_of<component::sigil_force>(entity);
            double force_sigil_addend =
                has_force_sigil * component::sigil_force::strike_damage_increase;

            bool has_impact_sigil = ctx.registry.any_of<component::sigil_impact>(entity);
            double impact_sigil_addend =
                has_impact_sigil * component::sigil_impact::strike_damage_increase;

            bool retribution_is_traited = ctx.registry.any_of<component::retribution>(entity);
            bool has_resolution = ctx.registry.any_of<component::resolution>(entity);
            double retribution_addend =
                retribution_is_traited * has_resolution *
                component::retribution::strike_damage_increase_if_has_resolution;

            bool unscathed_contender_is_traited =
                ctx.registry.any_of<component::unscathed_contender>(entity);
            bool has_aegis = ctx.registry.any_of<component::aegis>(entity);
            double unscathed_contender_addend =
                unscathed_contender_is_traited * has_aegis *
                component::unscathed_contender::strike_damage_increase_if_has_aegis;

            auto targeting_ptr = ctx.registry.try_get<component::targeting>(entity);
            double fiery_wrath_multiplier = 1.0;
            double symbolic_exposure_multiplier = 1.0;
            if (targeting_ptr) {
                bool fiery_wrath_is_traited = ctx.registry.any_of<component::fiery_wrath>(entity);
                bool target_is_burning =
                    ctx.registry.any_of<component::burning>(targeting_ptr->entity);
                fiery_wrath_multiplier +=
                    fiery_wrath_is_traited * target_is_burning *
                    component::fiery_wrath::strike_damage_increase_if_target_burning;

                bool symbolic_exposure_is_traited =
                    ctx.registry.any_of<component::symbolic_exposure>(entity);
                bool target_is_vulnerable =
                    ctx.registry.any_of<component::vulnerability>(targeting_ptr->entity);
                symbolic_exposure_multiplier +=
                    symbolic_exposure_is_traited * target_is_vulnerable *
                    component::symbolic_exposure::strike_damage_increase_if_target_vulnerable;

                // TODO: Add logic to check stunned target for impact sigil's second part
            }

            bool inspired_virtue_is_traited =
                ctx.registry.any_of<component::inspired_virtue>(entity);
            auto [aegis_ptr, might_ptr, fury_ptr, quickness_ptr, resolution_ptr] =
                ctx.registry.try_get<component::aegis,
                                     component::might,
                                     component::fury,
                                     component::quickness,
                                     component::resolution>(entity);
            unsigned int boon_count = (aegis_ptr != nullptr) + (might_ptr != nullptr) +
                                      (fury_ptr != nullptr) + (quickness_ptr != nullptr) +
                                      (resolution_ptr != nullptr);
            double inspired_virtue_multiplier =
                1.0 + (inspired_virtue_is_traited * boon_count *
                       component::inspired_virtue::strike_damage_increase_per_boon);

            double addends_multiplier = (1.0 + (force_sigil_addend + impact_sigil_addend +
                                                retribution_addend + unscathed_contender_addend));

            double critical_hit_multiplier =
                (1.0 + (std::min(effective_attributes.critical_chance_pct, 100.0) / 100.0) *
                           (effective_attributes.critical_damage_pct / 100.0 - 1.0));

            unsigned int weapon_strength = 1100;
            ctx.registry.emplace<component::outgoing_strike_damage_multiplier>(
                entity,
                component::outgoing_strike_damage_multiplier{
                    addends_multiplier * scholar_rune_multiplier * inspired_virtue_multiplier *
                    fiery_wrath_multiplier * symbolic_exposure_multiplier *
                    critical_hit_multiplier * effective_attributes.power * weapon_strength});
        });
}

}  // namespace gw2combat::system

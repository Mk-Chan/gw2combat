#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/animation.hpp"
#include "gw2combat/component/boon/aegis.hpp"
#include "gw2combat/component/boon/fury.hpp"
#include "gw2combat/component/boon/might.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/boon/resolution.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/outgoing_damage.hpp"
#include "gw2combat/component/profession/guardian/trait/fiery_wrath.hpp"
#include "gw2combat/component/successfully_cast_skill.hpp"
#include "gw2combat/component/targeting.hpp"

namespace gw2combat::system {

double get_damage_coefficient_by_skill(component::successfully_cast_skill::skill skill) {
    // TODO: Make a nicer implementation for this by storing all the relevant entity-based
    //       information together in 1 place
    if (skill == component::successfully_cast_skill::skill::SKILL_GUARDIAN_GREATSWORD_1_1) {
        return 0.8;
    } else if (skill == component::successfully_cast_skill::skill::SKILL_GUARDIAN_GREATSWORD_1_2) {
        return 0.8;
    } else if (skill == component::successfully_cast_skill::skill::SKILL_GUARDIAN_GREATSWORD_1_3) {
        return 1.2;
    } else {
        return 0;
    }
}

void outgoing_strike_damage_calculation(context& ctx) {
    ctx.registry.view<component::successfully_cast_skill, component::effective_attributes>().each(
        [&](const entt::entity entity,
            const component::successfully_cast_skill& successfully_cast_skill,
            const component::effective_attributes& effective_attributes) {
            auto targeting_ptr = ctx.registry.try_get<component::targeting>(entity);

            double damage_coefficient =
                get_damage_coefficient_by_skill(successfully_cast_skill.skill_);

            bool has_fiery_wrath = ctx.registry.any_of<component::fiery_wrath>(entity);
            bool target_is_burning = targeting_ptr != nullptr &&
                                     ctx.registry.any_of<component::burning>(targeting_ptr->entity);
            double fiery_wrath_multiplier =
                1.0 + (has_fiery_wrath * target_is_burning *
                       component::fiery_wrath::strike_damage_increase_pct);

            auto [aegis_ptr, might_ptr, fury_ptr, quickness_ptr, resolution_ptr] =
                ctx.registry.try_get<component::aegis,
                                     component::might,
                                     component::fury,
                                     component::quickness,
                                     component::resolution>(entity);
            unsigned int boon_count = (aegis_ptr != nullptr) + (might_ptr != nullptr) +
                                      (fury_ptr != nullptr) + (quickness_ptr != nullptr) +
                                      (resolution_ptr != nullptr);
            double inspired_virtues_multiplier = 1.0 + (boon_count * 0.01);

            auto calculate_dmg = [&](unsigned int weapon_strength) {
                return (1.0 + (0.05 + 0.03 + 0.1 + 0.2)) *  // force + impact + retribution + uc
                       (1.0 + (0.05)) *                     // scholar
                       inspired_virtues_multiplier *        // inspired virtues
                       fiery_wrath_multiplier *             // fiery wrath
                       (1.0 + (0.05)) *                     // symbolic exposure
                       (1.0 + (std::min(effective_attributes.critical_chance_pct, 100.0) / 100.0) *
                                  (effective_attributes.critical_damage_pct / 100.0 - 1.0)) *
                       weapon_strength * effective_attributes.power * damage_coefficient;
            };

            unsigned int avg_weapon_strength = 1100;
            ctx.registry.emplace_or_replace<component::outgoing_damage>(
                entity, component::outgoing_damage{calculate_dmg(avg_weapon_strength)});

            // NOTE: Logging purposes only
            unsigned int min_weapon_strength = 1045;
            unsigned int max_weapon_strength = 1155;
            spdlog::info("current tick: {}", ctx.current_tick);
            spdlog::info(
                "source: {}, assuming 2597 armor, 25 vuln, min_dmg: {}, avg_dmg: {}, max_dmg: {}",
                static_cast<std::uint32_t>(entity),
                calculate_dmg(min_weapon_strength) * 1.25 / 2597,
                calculate_dmg(avg_weapon_strength) * 1.25 / 2597,
                calculate_dmg(max_weapon_strength) * 1.25 / 2597);
        });
}

}  // namespace gw2combat::system

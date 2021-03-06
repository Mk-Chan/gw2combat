#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "system.hpp"

#include "gw2combat/component/boon/fury.hpp"
#include "gw2combat/component/boon/might.hpp"
#include "gw2combat/component/character/dynamic_attributes.hpp"
#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/character/static_attributes.hpp"

namespace gw2combat::system {

void effective_attributes_calculation(context& ctx) {
    ctx.registry.view<component::static_attributes, component::dynamic_attributes>().each(
        [&](const entt::entity entity,
            const component::static_attributes& static_attributes,
            const component::dynamic_attributes& dynamic_attributes) {
            auto& effective_attributes =
                ctx.registry.get_or_emplace<component::effective_attributes>(
                    entity,
                    component::effective_attributes{
                        .power = static_attributes.power,
                        .precision = static_attributes.precision,
                        .toughness = static_attributes.toughness,
                        .vitality = static_attributes.vitality,
                        .concentration = static_attributes.concentration,
                        .condition_damage = static_attributes.condition_damage,
                        .expertise = static_attributes.expertise,
                        .ferocity = static_attributes.ferocity,
                        .healing_power = static_attributes.healing_power,
                        .armor = static_attributes.armor,
                        .boon_duration_pct = static_attributes.boon_duration_pct,
                        .critical_chance_pct = static_attributes.critical_chance_pct,
                        .critical_damage_pct = static_attributes.critical_damage_pct,
                        .condition_duration_pct = static_attributes.condition_duration_pct,
                        .max_health = static_attributes.max_health,
                        .endurance = dynamic_attributes.max_endurance,
                        .endurance_gain_pct = dynamic_attributes.endurance_gain_pct});

            auto might_ptr = ctx.registry.try_get<component::might>(entity);
            if (might_ptr != nullptr) {
                effective_attributes.power += might_ptr->stacks.size() * 30;
                effective_attributes.condition_damage += might_ptr->stacks.size() * 30;
            }
            bool has_fury = ctx.registry.any_of<component::fury>(entity);
            if (has_fury) {
                effective_attributes.critical_chance_pct += 25;
            }

            // spdlog::info(
            //     "entity: {}, effective_attributes: {}",
            //     static_cast<std::uint32_t>(entity),
            //     nlohmann::json{
            //         {"power", effective_attributes.power},
            //         //{"precision", effective_attributes.precision},
            //         //{"toughness", effective_attributes.toughness},
            //         //{"vitality", effective_attributes.vitality},
            //         //{"concentration", effective_attributes.concentration},
            //         //{"condition_damage", effective_attributes.condition_damage},
            //         //{"expertise", effective_attributes.expertise},
            //         //{"ferocity", effective_attributes.ferocity},
            //         //{"healing_power", effective_attributes.healing_power},
            //         {"armor", effective_attributes.armor},
            //         //{"boon_duration_pct", effective_attributes.boon_duration_pct},
            //         {"critical_chance_pct", effective_attributes.critical_chance_pct},
            //         {"critical_damage_pct", effective_attributes.critical_damage_pct},
            //         //{"condition_duration_pct", effective_attributes.condition_duration_pct},
            //         //{"health", effective_attributes.health},
            //         {"weapon_strength", effective_attributes.weapon_strength},
            //         //{"endurance", effective_attributes.endurance},
            //         //{"endurance_gain_pct", effective_attributes.endurance_gain_pct},
            //     }
            //         .dump());
        });
}

}  // namespace gw2combat::system

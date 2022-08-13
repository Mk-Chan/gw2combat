#include "system.hpp"

#include "gw2combat/component/character/dynamic_attributes.hpp"
#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/character/static_attributes.hpp"
#include "gw2combat/component/gear/bundles.hpp"
#include "gw2combat/component/gear/runes.hpp"

namespace gw2combat::system {

void effective_attributes_calculation(registry_t& registry, tick_t) {
    registry.view<component::static_attributes, component::dynamic_attributes>().each(
        [&](entity_t entity,
            const component::static_attributes& static_attributes,
            const component::dynamic_attributes& dynamic_attributes) {
            auto& effective_attributes = registry.get_or_emplace<component::effective_attributes>(
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
                    .burning_duration_pct = static_attributes.burning_duration_pct,
                    .bleeding_duration_pct = static_attributes.bleeding_duration_pct,
                    .confusion_duration_pct = static_attributes.confusion_duration_pct,
                    .poison_duration_pct = static_attributes.poison_duration_pct,
                    .torment_duration_pct = static_attributes.torment_duration_pct,
                    .max_health = static_attributes.max_health,
                    .endurance = dynamic_attributes.max_endurance,
                    .endurance_gain_pct = dynamic_attributes.endurance_gain_pct});

            if (utils::has_effect(effects::effect_type::FURY, entity, registry)) {
                effective_attributes.critical_chance_pct += 25;
            }

            if (utils::has_trait(trait_type::RIGHTEOUS_INSTINCTS, entity, registry) &&
                utils::has_effect(effects::effect_type::RESOLUTION, entity, registry)) {
                effective_attributes.critical_chance_pct += 25;
            }
            if (utils::has_trait(trait_type::RIGHT_HAND_STRENGTH, entity, registry) &&
                !registry.any_of<component::bundle>(entity) &&
                utils::has_one_handed_weapon(weapon_position::MAIN_HAND, entity, registry)) {
                effective_attributes.power += 80;
            }
            if (utils::has_trait(trait_type::ZEALOUS_BLADE, entity, registry) &&
                !registry.any_of<component::bundle>(entity) &&
                utils::has_weapon_type(weapon_type::GREATSWORD, entity, registry)) {
                effective_attributes.power += 120;
            }
            if (utils::has_trait(trait_type::RADIANT_POWER, entity, registry)) {
                effective_attributes.ferocity += 150;
                effective_attributes.critical_damage_pct += 10.0;
            }
            if (utils::has_trait(trait_type::IMBUED_HASTE, entity, registry) &&
                utils::has_effect(effects::effect_type::QUICKNESS, entity, registry)) {
                effective_attributes.condition_damage += 250;
                effective_attributes.healing_power += 250;
                effective_attributes.vitality += 250;
            }
            if (utils::has_trait(trait_type::POWER_OF_THE_VIRTUOUS, entity, registry)) {
                effective_attributes.condition_damage +=
                    (unsigned int)(0.13 * (double)effective_attributes.vitality);
            }

            // FIXME: Remember to implement banker's rounding later
            if (registry.any_of<component::enhancement_component>(entity) &&
                registry.get<component::enhancement_component>(entity).enhancement ==
                    component::enhancement_type::TOXIC_FOCUSING_CRYSTAL) {
                effective_attributes.condition_damage +=
                    (unsigned int)((0.03 * (double)effective_attributes.power));
                effective_attributes.condition_damage +=
                    (unsigned int)(0.03 * (double)effective_attributes.precision);
            } else if (registry.any_of<component::enhancement_component>(entity) &&
                       registry.get<component::enhancement_component>(entity).enhancement ==
                           component::enhancement_type::SUPERIOR_SHARPENING_STONE) {
                effective_attributes.power +=
                    (unsigned int)((0.03 * (double)effective_attributes.precision));
                effective_attributes.power +=
                    (unsigned int)(0.06 * (double)effective_attributes.ferocity);
            }

            size_t might_stacks =
                utils::get_num_stacks_of_effect(effects::effect_type::MIGHT, entity, registry);
            effective_attributes.power += might_stacks * 30;
            effective_attributes.condition_damage += might_stacks * 30;

            // spdlog::info(
            //     "entity: {}, effective_attributes: {}",
            //     utils::get_entity_name(entity, registry),
            //     nlohmann::json{
            //         {"power", effective_attributes.power},
            //         {"precision", effective_attributes.precision},
            //         //{"toughness", effective_attributes.toughness},
            //         //{"vitality", effective_attributes.vitality},
            //         //{"concentration", effective_attributes.concentration},
            //         {"condition_damage", effective_attributes.condition_damage},
            //         {"expertise", effective_attributes.expertise},
            //         {"ferocity", effective_attributes.ferocity},
            //         //{"healing_power", effective_attributes.healing_power},
            //         {"armor", effective_attributes.armor},
            //         //{"boon_duration_pct", effective_attributes.boon_duration_pct},
            //         {"critical_chance_pct", effective_attributes.critical_chance_pct},
            //         {"critical_damage_pct", effective_attributes.critical_damage_pct},
            //         {"condition_duration_pct", effective_attributes.condition_duration_pct},
            //         {"burning_duration_pct", effective_attributes.burning_duration_pct},
            //         {"bleeding_duration_pct", effective_attributes.bleeding_duration_pct},
            //         //{"health", effective_attributes.health},
            //         //{"endurance", effective_attributes.endurance},
            //         //{"endurance_gain_pct", effective_attributes.endurance_gain_pct},
            //     }
            //         .dump());
        });
    // exit(0);
}

}  // namespace gw2combat::system

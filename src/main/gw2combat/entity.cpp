#include "entity.hpp"

#include "utilities.hpp"

#include "gw2combat/build.hpp"
#include "gw2combat/effects.hpp"

#include "gw2combat/component/character/dynamic_attributes.hpp"
#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/character/rotation.hpp"
#include "gw2combat/component/character/static_attributes.hpp"
#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/effect_components.hpp"
#include "gw2combat/component/profession_components.hpp"

namespace gw2combat {

entity_t build_cfb(registry_t& registry) {
    character_build build = character_build::read("src/main/resources/build-cfb.json");

    auto entity = registry.create();
    registry.emplace<component::static_attributes>(
        entity,
        component::static_attributes{
            .power = build.attributes.power,
            .precision = build.attributes.precision,
            .toughness = build.attributes.toughness,
            .vitality = build.attributes.vitality,
            .concentration = build.attributes.concentration,
            .condition_damage = build.attributes.condition_damage,
            .expertise = build.attributes.expertise,
            .ferocity = build.attributes.ferocity,
            .healing_power = build.attributes.healing_power,
            .armor = build.attributes.armor,
            .boon_duration_pct = build.attributes.boon_duration_pct,
            .critical_chance_pct = build.attributes.critical_chance_pct,
            .critical_damage_pct = build.attributes.critical_damage_pct,
            .condition_duration_pct = build.attributes.condition_duration_pct,
            .burning_duration_pct = build.attributes.burning_duration_pct,
            .bleeding_duration_pct = build.attributes.bleeding_duration_pct,
            .confusion_duration_pct = build.attributes.confusion_duration_pct,
            .poison_duration_pct = build.attributes.poison_duration_pct,
            .torment_duration_pct = build.attributes.torment_duration_pct,
            .max_health = build.attributes.max_health,
        });
    registry.emplace<component::dynamic_attributes>(
        entity, component::dynamic_attributes{.max_endurance = 100});

    if (build.base_class == base_class_type::GUARDIAN) {
        registry.emplace<component::virtue_of_justice>(entity, component::virtue_of_justice{5});
    }
    registry.emplace<component::traits_component>(entity,
                                                  component::traits_component{build.traits});
    if (utils::has_trait(trait_type::PERMEATING_WRATH, entity, registry)) {
        registry.get<component::virtue_of_justice>(entity).number_of_ticks_for_burning_application =
            3;
    }
    registry.emplace<component::rune_component>(entity, component::rune_component{build.rune});
    registry.emplace<component::available_weapon_configurations>(
        entity, build.available_weapon_configurations);
    registry.emplace<component::equipped_weapon_set>(
        entity, component::equipped_weapon_set{weapon_set::SET_1});

    registry.emplace<component::is_actor>(entity);
    registry.ctx().emplace_hint<entt::hashed_string>(to_u32(entity), "player_cfb"_hs);

    return entity;
}

entity_t build_core_guard(registry_t& registry) {
    character_build build = character_build::read("src/main/resources/build-core-guard.json");

    auto entity = registry.create();
    registry.emplace<component::static_attributes>(
        entity,
        component::static_attributes{
            .power = build.attributes.power,
            .precision = build.attributes.precision,
            .toughness = build.attributes.toughness,
            .vitality = build.attributes.vitality,
            .concentration = build.attributes.concentration,
            .condition_damage = build.attributes.condition_damage,
            .expertise = build.attributes.expertise,
            .ferocity = build.attributes.ferocity,
            .healing_power = build.attributes.healing_power,
            .armor = build.attributes.armor,
            .boon_duration_pct = build.attributes.boon_duration_pct,
            .critical_chance_pct = build.attributes.critical_chance_pct,
            .critical_damage_pct = build.attributes.critical_damage_pct,
            .condition_duration_pct = build.attributes.condition_duration_pct,
            .burning_duration_pct = build.attributes.burning_duration_pct,
            .bleeding_duration_pct = build.attributes.bleeding_duration_pct,
            .confusion_duration_pct = build.attributes.confusion_duration_pct,
            .poison_duration_pct = build.attributes.poison_duration_pct,
            .torment_duration_pct = build.attributes.torment_duration_pct,
            .max_health = build.attributes.max_health,
        });
    registry.emplace<component::dynamic_attributes>(
        entity, component::dynamic_attributes{.max_endurance = 100});

    if (build.base_class == base_class_type::GUARDIAN) {
        registry.emplace<component::virtue_of_justice>(entity, component::virtue_of_justice{5});
    }
    registry.emplace<component::traits_component>(entity,
                                                  component::traits_component{build.traits});
    if (utils::has_trait(trait_type::PERMEATING_WRATH, entity, registry)) {
        registry.get<component::virtue_of_justice>(entity).number_of_ticks_for_burning_application =
            3;
    }
    registry.emplace<component::rune_component>(entity, component::rune_component{build.rune});
    registry.emplace<component::available_weapon_configurations>(
        entity, build.available_weapon_configurations);
    registry.emplace<component::equipped_weapon_set>(
        entity, component::equipped_weapon_set{weapon_set::SET_1});

    registry.emplace<component::is_actor>(entity);
    registry.ctx().emplace_hint<entt::hashed_string>(to_u32(entity), "player_core_guard"_hs);

    return entity;
}

entity_t build_medium_kitty_golem(entt::registry& registry) {
    auto entity = registry.create();

    registry.emplace<component::static_attributes>(
        entity, component::static_attributes{.armor = 2597, .max_health = 1'000'000});
    registry.emplace<component::dynamic_attributes>(entity);

    registry.emplace<component::is_actor>(entity);
    registry.ctx().emplace_hint<entt::hashed_string>(to_u32(entity), "medium_kitty_golem"_hs);

    return entity;
}

entity_t build_golem_boon_condi_provider(entt::registry& registry) {
    auto entity = registry.create();

    registry.emplace<component::static_attributes>(
        entity, component::static_attributes{.condition_damage = 0});
    registry.emplace<component::dynamic_attributes>(entity);

    registry.emplace<component::is_actor>(entity);
    registry.ctx().emplace_hint<entt::hashed_string>(to_u32(entity),
                                                     "golem_boon_condi_provider"_hs);

    return entity;
}

void init_entities(entt::registry& registry) {
    auto player1 = build_core_guard(registry);
    // auto player1 = build_cfb(registry);
    auto golem = build_medium_kitty_golem(registry);
    auto golem_boon_condi_provider = build_golem_boon_condi_provider(registry);

    // NOTE: Default boons for testing
    using namespace effects;
    std::vector<skill_cast> player1_rotation = read_rotation("src/main/resources/rotation.csv");
    registry.emplace<component::rotation>(player1, component::rotation{player1_rotation});
    registry.emplace<component::aegis>(
        player1, component::aegis{aegis{golem_boon_condi_provider, 0, 1'000'000'000}});
    registry.emplace<component::alacrity>(
        player1, component::alacrity{alacrity{golem_boon_condi_provider, 0, 1'000'000'000}});
    registry.emplace<component::fury>(
        player1, component::fury{fury{golem_boon_condi_provider, 0, 1'000'000'000}});
    registry.emplace<component::might>(player1,
                                       component::might{stacking_effect<might>{
                                           golem_boon_condi_provider, 25, 0, 1'000'000'000, 25}});
    registry.emplace<component::quickness>(
        player1, component::quickness{quickness{golem_boon_condi_provider, 0, 1'000'000'000}});
    registry.emplace<component::resolution>(
        player1, component::resolution{resolution{golem_boon_condi_provider, 0, 1'000'000'000}});

    registry.emplace<component::targeting>(player1, component::targeting{golem});
    // registry.remove<component::is_character>(player1);  // Disable player1

    registry.emplace<component::burning>(golem,
                                         component::burning{stacking_effect<burning>{
                                             golem_boon_condi_provider, 1, 0, 1'000'000'000}});
    registry.emplace<component::vulnerability>(
        golem,
        component::vulnerability{
            stacking_effect<vulnerability>{golem_boon_condi_provider, 25, 0, 1'000'000'000, 25}});
}

}  // namespace gw2combat

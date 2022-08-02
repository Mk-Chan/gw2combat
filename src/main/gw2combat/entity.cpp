#include "entity.hpp"

#include "gw2combat/effects.hpp"

#include "gw2combat/component/character/dynamic_attributes.hpp"
#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/character/rotation.hpp"
#include "gw2combat/component/character/static_attributes.hpp"
#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/effect_components.hpp"
#include "gw2combat/component/gear/rune/rune_balthazar.hpp"
#include "gw2combat/component/gear/rune/rune_scholar.hpp"
#include "gw2combat/component/gear/sigil/sigil_force.hpp"
#include "gw2combat/component/gear/sigil/sigil_impact.hpp"
#include "gw2combat/component/profession_components.hpp"
#include "gw2combat/component/trait_components.hpp"

namespace gw2combat {

entity_t build_cfb(registry_t& registry) {
    auto entity = registry.create();
    auto cfb_gear_food_util = component::static_attributes{.power = 2159,
                                                           .precision = 1842,
                                                           .toughness = 1000,
                                                           .vitality = 1235,
                                                           .concentration = 0,
                                                           .condition_damage = 2088,
                                                           .expertise = 451,
                                                           .ferocity = 150,
                                                           .healing_power = 0,
                                                           .armor = 2271,
                                                           .boon_duration_pct = 0.0,
                                                           .critical_chance_pct = 45.09,
                                                           .critical_damage_pct = 160.0,
                                                           .condition_duration_pct = 30.06,
                                                           .max_health = 15394};
    registry.emplace<component::static_attributes>(entity, cfb_gear_food_util);
    registry.emplace<component::dynamic_attributes>(
        entity, component::dynamic_attributes{.max_endurance = 100});

    // Replace with correct components
    registry.emplace<component::virtue_of_justice>(entity, component::virtue_of_justice{3});
    registry.emplace<component::fiery_wrath>(entity);
    registry.emplace<component::inspired_virtue>(entity);
    registry.emplace<component::retribution>(entity);
    registry.emplace<component::symbolic_power>(entity);
    registry.emplace<component::symbolic_exposure>(entity);
    registry.emplace<component::symbolic_avenger_trait>(entity);
    registry.emplace<component::unscathed_contender>(entity);
    registry.emplace<component::sigil_force>(entity);
    registry.emplace<component::sigil_impact>(entity);
    // Replace with correct components - end

    registry.emplace<component::rune_balthazar>(entity);

    registry.emplace<component::is_actor>(entity);
    registry.ctx().emplace_hint<entt::hashed_string>(to_u32(entity), "player1"_hs);

    return entity;
}

entity_t build_player1(registry_t& registry) {
    auto entity = registry.create();

    auto viper_armor_rest_zerk_crit_food =
        component::static_attributes{.power = 3004,
                                     .precision = 2032,
                                     .toughness = 1000,
                                     .vitality = 1235,
                                     .concentration = 0,
                                     .condition_damage = 1287,
                                     .expertise = 207,
                                     .ferocity = 1090,
                                     .healing_power = 0,
                                     .armor = 2271,
                                     .boon_duration_pct = 0.0,
                                     .critical_chance_pct = 79.1,
                                     .critical_damage_pct = 222.6,
                                     .condition_duration_pct = 13.8,
                                     .max_health = 13995};
    auto full_zerk_crit_food = component::static_attributes{.power = 3067,
                                                            .precision = 2140,
                                                            .toughness = 1000,
                                                            .vitality = 1235,
                                                            .concentration = 0,
                                                            .condition_damage = 161,
                                                            .expertise = 0,
                                                            .ferocity = 1435,
                                                            .healing_power = 0,
                                                            .armor = 2271,
                                                            .boon_duration_pct = 0.0,
                                                            .critical_chance_pct = 84.2,
                                                            .critical_damage_pct = 243.6,
                                                            .condition_duration_pct = 0.0,
                                                            .max_health = 14615};
    auto full_zerk = component::static_attributes{.power = 3067,
                                                  .precision = 2040,
                                                  .toughness = 1000,
                                                  .vitality = 1235,
                                                  .concentration = 0,
                                                  .condition_damage = 161,
                                                  .expertise = 0,
                                                  .ferocity = 1335,
                                                  .healing_power = 0,
                                                  .armor = 2271,
                                                  .boon_duration_pct = 0.0,
                                                  .critical_chance_pct = 79.5,
                                                  .critical_damage_pct = 238.9,
                                                  .condition_duration_pct = 0.0,
                                                  .max_health = 14615};
    registry.emplace<component::static_attributes>(entity, full_zerk);
    registry.emplace<component::dynamic_attributes>(
        entity, component::dynamic_attributes{.max_endurance = 100});

    registry.emplace<component::virtue_of_justice>(entity, component::virtue_of_justice{3});
    registry.emplace<component::fiery_wrath>(entity);
    registry.emplace<component::inspired_virtue>(entity);
    registry.emplace<component::retribution>(entity);
    registry.emplace<component::symbolic_power>(entity);
    registry.emplace<component::symbolic_exposure>(entity);
    registry.emplace<component::symbolic_avenger_trait>(entity);
    registry.emplace<component::unscathed_contender>(entity);
    registry.emplace<component::sigil_force>(entity);
    registry.emplace<component::sigil_impact>(entity);
    registry.emplace<component::rune_scholar>(entity);

    registry.emplace<component::is_actor>(entity);
    registry.ctx().emplace_hint<entt::hashed_string>(to_u32(entity), "player1"_hs);

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
    auto player1 = build_player1(registry);
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

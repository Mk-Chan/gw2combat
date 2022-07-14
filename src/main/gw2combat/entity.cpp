#include "entity.hpp"

#include "gw2combat/effect.hpp"

#include "gw2combat/component/boon/aegis.hpp"
#include "gw2combat/component/boon/alacrity.hpp"
#include "gw2combat/component/boon/fury.hpp"
#include "gw2combat/component/boon/might.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/boon/resolution.hpp"
#include "gw2combat/component/character/dynamic_attributes.hpp"
#include "gw2combat/component/character/is_character.hpp"
#include "gw2combat/component/character/static_attributes.hpp"
#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/condition/vulnerability.hpp"
#include "gw2combat/component/gear/rune/rune_scholar.hpp"
#include "gw2combat/component/gear/sigil/sigil_force.hpp"
#include "gw2combat/component/gear/sigil/sigil_impact.hpp"
#include "gw2combat/component/profession/virtue_of_justice.hpp"
#include "gw2combat/component/traits/guardian/fiery_wrath.hpp"
#include "gw2combat/component/traits/guardian/inspired_virtue.hpp"
#include "gw2combat/component/traits/guardian/retribution.hpp"
#include "gw2combat/component/traits/guardian/symbolic_exposure.hpp"
#include "gw2combat/component/traits/guardian/unscathed_contender.hpp"

namespace gw2combat {

std::unique_ptr<entt::entity> singleton_entity;

entt::entity build_player1(entt::registry& registry) {
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
        entity, component::dynamic_attributes{.weapon_strength = 1100, .max_endurance = 100});

    registry.emplace<component::virtue_of_justice>(entity, component::virtue_of_justice{3});
    registry.emplace<component::fiery_wrath>(entity);
    registry.emplace<component::inspired_virtue>(entity);
    registry.emplace<component::retribution>(entity);
    registry.emplace<component::symbolic_exposure>(entity);
    registry.emplace<component::unscathed_contender>(entity);
    registry.emplace<component::sigil_force>(entity);
    registry.emplace<component::sigil_impact>(entity);
    registry.emplace<component::rune_scholar>(entity);

    registry.emplace<component::is_character>(entity);

    return entity;
}

entt::entity build_medium_kitty_golem(entt::registry& registry) {
    auto entity = registry.create();

    registry.emplace<component::static_attributes>(
        entity, component::static_attributes{.armor = 2597, .max_health = 4'000'000});
    registry.emplace<component::dynamic_attributes>(entity);

    registry.emplace<component::is_character>(entity);

    return entity;
}

entt::entity build_golem_boon_condi_provider(entt::registry& registry) {
    auto entity = registry.create();

    registry.emplace<component::static_attributes>(
        entity, component::static_attributes{.condition_damage = 0});
    registry.emplace<component::dynamic_attributes>(entity);

    registry.emplace<component::is_character>(entity);

    return entity;
}

void init_entities(entt::registry& registry) {
    singleton_entity = std::make_unique<entt::entity>(registry.create());
    auto player1 = build_player1(registry);
    auto golem = build_medium_kitty_golem(registry);
    auto golem_boon_condi_provider = build_golem_boon_condi_provider(registry);

    // NOTE: Default boons for testing
    registry.emplace<component::might>(player1, component::might{player1, 25, 0, 35'000'000});
    registry.emplace<component::fury>(player1, component::fury{effect{player1, 0, 35'000'000}});
    registry.emplace<component::quickness>(player1,
                                           component::quickness{effect{player1, 0, 35'000'000}});
    registry.emplace<component::alacrity>(player1,
                                          component::alacrity{effect{player1, 0, 35'000'000}});
    registry.emplace<component::resolution>(player1,
                                            component::resolution{effect{player1, 0, 35'000'000}});
    registry.emplace<component::aegis>(player1, component::aegis{effect{player1, 0, 35'000'000}});
    registry.emplace<component::targeting>(player1, component::targeting{golem});
    // registry.remove<component::is_character>(player1);  // Disable player1

    registry.emplace<component::burning>(
        golem, component::burning{golem_boon_condi_provider, 1, 0, 35'000'000});
    registry.emplace<component::vulnerability>(
        golem, component::vulnerability{golem_boon_condi_provider, 25, 0, 35'000'000});
}

}  // namespace gw2combat

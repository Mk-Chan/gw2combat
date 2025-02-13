#include "encounter.hpp"

#include "audit.hpp"

#include "actor/rotation.hpp"

#include "component/actor/base_class_component.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/profession_component.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/actor/team.hpp"
#include "component/audit/audit_component.hpp"
#include "component/counter/is_counter.hpp"
#include "component/counter/is_counter_modifier.hpp"
#include "component/encounter/encounter_configuration_component.hpp"
#include "component/equipment/weapons.hpp"

#include "configuration/build.hpp"
#include "configuration/encounter.hpp"

#include "utils/actor_utils.hpp"
#include "utils/io_utils.hpp"

namespace gw2combat::system {

void add_recipe_items_to_actor(
    const std::vector<configuration::counter_configuration_t>& counters,
    const std::vector<actor::effect_t>& permanent_effects,
    const std::vector<configuration::unique_effect_t>& permanent_unique_effects,
    const std::vector<configuration::skill_t>& skills,
    const std::vector<configuration::conditional_skill_group_t>& conditional_skill_groups,
    entity_t actor_entity,
    registry_t& registry) {
    for (auto& counter_configuration : counters) {
        for (auto&& [counter_entity, is_counter] : registry.view<component::is_counter>().each()) {
            if (counter_configuration.counter_key == is_counter.counter_configuration.counter_key) {
                throw std::runtime_error("multiple counters with the same name are not allowed");
            }
        }

        auto counter_entity = registry.create();
        registry.emplace<component::owner_component>(counter_entity, actor_entity);
        registry.emplace<component::is_counter>(
            counter_entity,
            component::is_counter{counter_configuration.initial_value, counter_configuration});
        utils::add_owner_based_component<std::vector<configuration::counter_modifier_t>,
                                         component::is_counter_modifier_t>(
            counter_configuration.counter_modifiers, actor_entity, registry);
    }
    for (auto& permanent_effect : permanent_effects) {
        utils::add_permanent_effect_to_actor(permanent_effect, actor_entity, registry);
    }
    for (auto& permanent_unique_effect : permanent_unique_effects) {
        utils::add_permanent_unique_effect_to_actor(
            permanent_unique_effect, actor_entity, registry);
    }
    for (auto& skill : skills) {
        utils::add_skill_to_actor(skill, actor_entity, registry);
    }
    for (auto& conditional_skill_group : conditional_skill_groups) {
        utils::add_conditional_skill_group_to_actor(
            conditional_skill_group, actor_entity, registry);
    }
}

void setup_encounter(registry_t& registry, const configuration::encounter_t& encounter) {
    auto singleton_entity = registry.create();
    registry.ctx().emplace_as<std::string>(singleton_entity, "Console");

    registry.emplace<component::encounter_configuration_component>(singleton_entity, encounter);
    registry.emplace<component::is_actor>(singleton_entity);
    registry.emplace<component::static_attributes>(
        singleton_entity, component::static_attributes{configuration::build_t{}.attributes});

    auto& audit_component = registry.emplace<component::audit_component>(
        singleton_entity,
        component::audit_component{
            .audit_configuration = encounter.audit_configuration,
            .events = {},
            .afk_ticks_by_actor = {}
        });
    // NOTE: This system is responsible for its own audit because it doesn't run in the combat loop
    audit_component.events.emplace_back(
        create_tick_event(audit::actor_created_event_t{}, singleton_entity, registry));

    for (auto&& actor : encounter.actors) {
        auto build = actor.build;

        auto actor_entity = registry.create();
        registry.ctx().emplace_as<std::string>(actor_entity, actor.name);

        registry.emplace<component::is_actor>(actor_entity);
        registry.emplace<component::team>(actor_entity, actor.team);
        registry.emplace<component::base_class_component>(actor_entity, build.base_class);
        registry.emplace<component::profession_component>(actor_entity, build.profession);
        registry.emplace<component::current_weapon_set>(actor_entity, build.initial_weapon_set);
        registry.emplace<component::static_attributes>(actor_entity, build.attributes);

        auto& equipped_weapons = registry.emplace<component::equipped_weapons>(actor_entity);
        for (auto& weapon_configuration : build.weapons) {
            equipped_weapons.weapons.emplace_back(
                component::weapon_t{.type = weapon_configuration.type,
                                    .position = weapon_configuration.position,
                                    .set = weapon_configuration.set});
        }
        for (auto& recipe : build.recipes) {
            add_recipe_items_to_actor(recipe.counters,
                                      recipe.permanent_effects,
                                      recipe.permanent_unique_effects,
                                      recipe.skills,
                                      recipe.conditional_skill_groups,
                                      actor_entity,
                                      registry);
        }
        for (auto& recipe_path : build.recipe_paths) {
            auto recipe = utils::read<configuration::recipe_t>(recipe_path);
            add_recipe_items_to_actor(recipe.counters,
                                      recipe.permanent_effects,
                                      recipe.permanent_unique_effects,
                                      recipe.skills,
                                      recipe.conditional_skill_groups,
                                      actor_entity,
                                      registry);
        }
        add_recipe_items_to_actor(build.counters,
                                  build.permanent_effects,
                                  build.permanent_unique_effects,
                                  build.skills,
                                  build.conditional_skill_groups,
                                  actor_entity,
                                  registry);

        if (!actor.rotation.skill_casts.empty()) {
            actor::rotation_t converted_rotation{};
            int offset = 0;
            bool first = true;
            for (auto&& skill_cast : actor.rotation.skill_casts) {
                if (first) {
                    offset = std::min(skill_cast.cast_time_ms, tick_t{0});
                    first = false;
                }
                converted_rotation.skill_casts.emplace_back(actor::skill_cast_t{
                    skill_cast.skill, (tick_t)(skill_cast.cast_time_ms - offset)});
            }
            registry.emplace<component::rotation_component>(
                actor_entity,
                component::rotation_component{converted_rotation, 0, 0, actor.rotation.repeat});
        }

        audit_component.events.emplace_back(
            create_tick_event(audit::actor_created_event_t{}, actor_entity, registry));
    }
}

}  // namespace gw2combat::system

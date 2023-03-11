#include "encounter.hpp"

#include "component/actor/base_class_component.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/profession_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/actor/team.hpp"
#include "component/audit/audit_component.hpp"
#include "component/counter/is_counter.hpp"
#include "component/equipment/weapons.hpp"

#include "configuration/build.hpp"
#include "configuration/encounter-local.hpp"
#include "configuration/encounter.hpp"

#include "utils/actor_utils.hpp"
#include "utils/io_utils.hpp"

namespace gw2combat::system {

void setup_server_encounter(registry_t& registry, const configuration::encounter_t& encounter) {
    auto singleton_entity = registry.create();
    registry.emplace<component::is_actor>(singleton_entity);
    registry.emplace<component::static_attributes>(
        singleton_entity, component::static_attributes{configuration::build_t{}.attributes});
    registry.ctx().emplace_as<std::string>(singleton_entity, "Console");

    for (auto&& actor : encounter.actors) {
        auto build = actor.build;

        auto actor_entity = registry.create();
        registry.ctx().emplace_as<std::string>(actor_entity, actor.name);
        registry.emplace<component::is_actor>(actor_entity);
        registry.emplace<component::audit_component>(actor_entity);
        registry.emplace<component::team>(actor_entity, actor.team);
        registry.emplace<component::base_class_component>(actor_entity, build.base_class);
        registry.emplace<component::profession_component>(actor_entity, build.profession);
        registry.emplace<component::current_weapon_set>(actor_entity);
        registry.emplace<component::static_attributes>(actor_entity, build.attributes);

        auto& equipped_weapons = registry.emplace<component::equipped_weapons>(actor_entity);
        for (auto& weapon_configuration : build.weapons) {
            equipped_weapons.weapons.emplace_back(
                component::weapon_t{.type = weapon_configuration.type,
                                    .position = weapon_configuration.position,
                                    .set = weapon_configuration.set});
        }
        for (auto& skill : build.skills) {
            utils::add_skill_to_actor(skill, actor_entity, registry);
        }
        for (auto& permanent_effect : build.permanent_effects) {
            utils::add_effect_to_actor(permanent_effect, actor_entity, registry);
        }
        for (auto& permanent_unique_effect : build.permanent_unique_effects) {
            utils::add_unique_effect_to_actor(permanent_unique_effect, actor_entity, registry);
        }
        for (auto& counter_configuration : build.counters) {
            for (auto&& [counter_entity, is_counter] :
                 registry.view<component::is_counter>().each()) {
                if (counter_configuration.counter_key ==
                    is_counter.counter_configuration.counter_key) {
                    throw std::runtime_error(
                        "multiple counters with the same name are not allowed");
                }
            }

            auto counter_entity = registry.create();
            registry.emplace<component::owner_component>(counter_entity, actor_entity);
            registry.emplace<component::is_counter>(
                counter_entity,
                component::is_counter{counter_configuration.initial_value, counter_configuration});
        }

        if (!actor.rotation.skill_casts.empty()) {
            actor::rotation_t converted_rotation{};
            for (auto&& skill_cast : actor.rotation.skill_casts) {
                converted_rotation.skill_casts.emplace_back(
                    actor::skill_cast_t{skill_cast.skill, skill_cast.cast_time_ms});
            }
            registry.emplace<component::rotation_component>(
                actor_entity,
                component::rotation_component{converted_rotation, 0, 0, actor.rotation.repeat});
        }
    }
}

}  // namespace gw2combat::system

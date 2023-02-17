#include "encounter.hpp"

#include "component/actor/base_class_component.hpp"
#include "component/actor/effects_component.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/profession_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/actor/team.hpp"
#include "component/equipment/weapons.hpp"

#include "configuration/build.hpp"
#include "configuration/encounter.hpp"
#include "configuration/rotation.hpp"

#include "utils/actor_utils.hpp"
#include "utils/io_utils.hpp"

namespace gw2combat::system {

void setup_encounter(registry_t& registry) {
    auto encounter = utils::read<configuration::encounter_t>("resources/encounter.json");
    for (auto&& actor : encounter.actors) {
        auto build = utils::read<configuration::build_t>(actor.build_path);

        auto actor_entity = registry.create();
        registry.ctx().emplace_as<std::string>(actor_entity, actor.name);
        registry.emplace<component::is_actor>(actor_entity);
        registry.emplace<component::team>(actor_entity, actor.team);
        registry.emplace<component::base_class_component>(actor_entity, build.base_class);
        registry.emplace<component::profession_component>(actor_entity, build.profession);
        registry.emplace<component::effects_component>(actor_entity);
        registry.emplace<component::current_weapon_set>(actor_entity);
        registry.emplace<component::strike_counter>(actor_entity);
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
        for (auto& permanent_unique_effects : build.permanent_unique_effects) {
            utils::add_unique_effect_to_actor(permanent_unique_effects, actor_entity, registry);
        }

        if (!actor.rotation_path.empty()) {
            auto rotation = utils::read<configuration::rotation_t>(actor.rotation_path);
            actor::rotation_t converted_rotation{};
            for (auto&& skill_cast : rotation.skill_casts) {
                converted_rotation.skill_casts.emplace_back(
                    actor::skill_cast_t{skill_cast.skill, skill_cast.cast_time_ms});
            }
            registry.emplace<component::rotation_component>(
                actor_entity,
                component::rotation_component{converted_rotation, 0, 0, rotation.repeat});
        }
    }
}

}  // namespace gw2combat::system
#include "encounter.hpp"

#include "component/actor/base_class_component.hpp"
#include "component/actor/counters_component.hpp"
#include "component/actor/effects_component.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/profession_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/actor/team.hpp"
#include "component/audit/audit_component.hpp"
#include "component/counter/is_counter.hpp"
#include "component/equipment/weapons.hpp"

#include "configuration/build.hpp"
#include "configuration/encounter-server.hpp"
#include "configuration/encounter.hpp"
#include "configuration/rotation.hpp"

#include "utils/actor_utils.hpp"
#include "utils/io_utils.hpp"

namespace gw2combat::system {

void setup_local_encounter(registry_t& registry, const configuration::encounter_t& encounter) {
    auto console_entity = registry.create();
    registry.emplace<component::is_actor>(console_entity);
    registry.emplace<component::static_attributes>(
        console_entity, component::static_attributes{configuration::build_t{}.attributes});
    registry.ctx().emplace_as<std::string>(console_entity, "Console");

    for (auto&& actor : encounter.actors) {
        auto build = utils::read<configuration::build_t>(actor.build_path);

        auto actor_entity = registry.create();
        registry.ctx().emplace_as<std::string>(actor_entity, actor.name);
        registry.emplace<component::is_actor>(actor_entity);
        if (!actor.audit_output_base_path.empty()) {
            auto& audit_component = registry.emplace<component::audit_component>(actor_entity);
            audit_component.audit_base_path = actor.audit_output_base_path;
        }
        registry.emplace<component::team>(actor_entity, actor.team);
        registry.emplace<component::base_class_component>(actor_entity, build.base_class);
        registry.emplace<component::profession_component>(actor_entity, build.profession);
        registry.emplace<component::effects_component>(actor_entity);
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

        auto& counters_component = registry.emplace<component::counters_component>(actor_entity);
        for (auto& counter_configuration : build.counters) {
            if (counters_component.has(counter_configuration.counter_key)) {
                throw std::runtime_error("multiple counters with the same name are not allowed");
            }

            auto counter_entity = registry.create();
            registry.emplace<component::owner_component>(counter_entity,
                                                         component::owner_component{actor_entity});
            registry.emplace<component::is_counter>(
                counter_entity,
                component::is_counter{counter_configuration.initial_value, counter_configuration});
            counters_component.counter_entities.emplace_back(
                component::counter_entity{counter_configuration.counter_key, counter_entity});
        }

        if (!actor.rotation_path.empty()) {
            if (actor.rotation_path.ends_with(".json")) {
                auto rotation = utils::read<configuration::rotation_t>(actor.rotation_path);
                actor::rotation_t converted_rotation{};
                for (auto&& skill_cast : rotation.skill_casts) {
                    converted_rotation.skill_casts.emplace_back(
                        actor::skill_cast_t{skill_cast.skill, skill_cast.cast_time_ms});
                }
                registry.emplace<component::rotation_component>(
                    actor_entity,
                    component::rotation_component{converted_rotation, 0, 0, rotation.repeat});
            } else if (actor.rotation_path.ends_with(".csv")) {
                std::basic_ifstream<char> ifstream{actor.rotation_path, std::basic_ios<char>::in};
                const auto file_size = std::filesystem::file_size(actor.rotation_path);
                std::string file_contents(file_size, '\0');
                ifstream.read(file_contents.data(), file_size);

                std::string delimiter = "\n";
                size_t last = 0;
                size_t next;
                std::string line;
                actor::rotation_t rotation;
                while ((next = file_contents.find(delimiter, last)) != std::string::npos) {
                    line = file_contents.substr(last, next - last);
                    size_t comma_pos = line.find(',');
                    if (comma_pos != std::string::npos) {
                        std::string skill_key = line.substr(0, comma_pos);
                        rotation.skill_casts.emplace_back(
                            actor::skill_cast_t{actor::skill_t{skill_key}});
                    }
                    last = next + 1;
                }
                line = file_contents.substr(last);
                if (!line.empty()) {
                    size_t comma_pos = line.find(',');
                    if (comma_pos != std::string::npos) {
                        std::string skill_key = line.substr(0, comma_pos);
                        rotation.skill_casts.emplace_back(
                            actor::skill_cast_t{actor::skill_t{skill_key}});
                    }
                }
                registry.emplace<component::rotation_component>(
                    actor_entity, component::rotation_component{rotation, 0, 0, false});
            }
        }
    }
}

void setup_server_encounter(registry_t& registry,
                            const configuration::encounter_server_t& encounter) {
    auto console_entity = registry.create();
    registry.emplace<component::is_actor>(console_entity);
    registry.emplace<component::static_attributes>(
        console_entity, component::static_attributes{configuration::build_t{}.attributes});
    registry.ctx().emplace_as<std::string>(console_entity, "Console");

    for (auto&& actor : encounter.actors) {
        auto build = actor.build;

        auto actor_entity = registry.create();
        registry.ctx().emplace_as<std::string>(actor_entity, actor.name);
        registry.emplace<component::is_actor>(actor_entity);
        registry.emplace<component::audit_component>(actor_entity);
        registry.emplace<component::team>(actor_entity, actor.team);
        registry.emplace<component::base_class_component>(actor_entity, build.base_class);
        registry.emplace<component::profession_component>(actor_entity, build.profession);
        registry.emplace<component::effects_component>(actor_entity);
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

        auto& counters_component = registry.emplace<component::counters_component>(actor_entity);
        for (auto& counter_configuration : build.counters) {
            if (counters_component.has(counter_configuration.counter_key)) {
                throw std::runtime_error("multiple counters with the same name are not allowed");
            }

            auto counter_entity = registry.create();
            registry.emplace<component::owner_component>(counter_entity,
                                                         component::owner_component{actor_entity});
            registry.emplace<component::is_counter>(
                counter_entity,
                component::is_counter{counter_configuration.initial_value, counter_configuration});
            counters_component.counter_entities.emplace_back(
                component::counter_entity{counter_configuration.counter_key, counter_entity});
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

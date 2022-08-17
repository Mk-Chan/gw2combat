#include "combat_loop.hpp"

#include "common.hpp"

#include "utilities/effect_utilities.hpp"
#include "utilities/entity_utilities.hpp"
#include "utilities/logging_utilities.hpp"

#include "system/system.hpp"

#include "actor/build.hpp"
#include "actor/rotation.hpp"
#include "actor/skill.hpp"

#include "gw2combat/component/actor/alacrity.hpp"
#include "gw2combat/component/actor/animation.hpp"
#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/finished_casting_skill.hpp"
#include "gw2combat/component/actor/is_actor.hpp"
#include "gw2combat/component/actor/no_more_rotation.hpp"
#include "gw2combat/component/actor/quickness.hpp"
#include "gw2combat/component/actor/rotation_component.hpp"
#include "gw2combat/component/actor/skills_component.hpp"
#include "gw2combat/component/actor/static_attributes.hpp"
#include "gw2combat/component/actor/team.hpp"
#include "gw2combat/component/attribute_modifications.hpp"
#include "gw2combat/component/equipment/weapons.hpp"
#include "gw2combat/component/owner_actor.hpp"
#include "gw2combat/component/skill/is_skill.hpp"

namespace gw2combat {

void build_actor(registry_t& registry,
                 const std::string& configuration_suffix,
                 int team_id,
                 actor::rotation_t* provided_rotation_ptr = nullptr) {
    auto actor_entity = registry.create();
    registry.ctx().emplace_hint<std::string>(
        actor_entity, fmt::format("actor{}-{}", actor_entity, configuration_suffix));
    registry.emplace<component::is_actor>(actor_entity);
    registry.emplace<component::quickness>(actor_entity);

    auto actor_build = actor::build_t::read(fmt::format("build-{}.json", configuration_suffix));
    auto base_class = actor_build.base_class;

    auto& skills_component = registry.emplace<component::skills_component>(actor_entity);

    auto& skill_database = actor::skill_database::instance();
    if (!actor_build.trait_lines.empty() &&
        actor_build.trait_lines[actor_build.trait_lines.size() - 1] ==
            actor::trait_line_t::FIREBRAND) {
        auto tome_of_justice_skill_entity = registry.create();
        registry.emplace<component::is_skill>(tome_of_justice_skill_entity);
        registry.ctx().emplace_hint<std::string>(tome_of_justice_skill_entity, "Tome of Justice");
        registry.emplace<component::owner_actor>(tome_of_justice_skill_entity,
                                                 component::owner_actor{actor_entity});
        skills_component.skill_entities.emplace_back(component::skill_entity{
            skill_database.find_by(actor::skill_t{base_class, "Tome of Justice"}).skill_key,
            tome_of_justice_skill_entity});
    } else if (base_class == actor::base_class_t::GUARDIAN) {
        auto virtue_of_justice_skill_entity = registry.create();
        registry.emplace<component::is_skill>(virtue_of_justice_skill_entity);
        registry.ctx().emplace_hint<std::string>(virtue_of_justice_skill_entity,
                                                 "Virtue of Justice");
        registry.emplace<component::owner_actor>(virtue_of_justice_skill_entity,
                                                 component::owner_actor{actor_entity});
        skills_component.skill_entities.emplace_back(component::skill_entity{
            skill_database.find_by(actor::skill_t{base_class, "Virtue of Justice"}).skill_key,
            virtue_of_justice_skill_entity});

        auto virtue_of_resolve_skill_entity = registry.create();
        registry.emplace<component::is_skill>(virtue_of_resolve_skill_entity);
        registry.ctx().emplace_hint<std::string>(virtue_of_resolve_skill_entity,
                                                 "Virtue of Resolve");
        registry.emplace<component::owner_actor>(virtue_of_resolve_skill_entity,
                                                 component::owner_actor{actor_entity});
        skills_component.skill_entities.emplace_back(component::skill_entity{
            skill_database.find_by(actor::skill_t{base_class, "Virtue of Resolve"}).skill_key,
            virtue_of_resolve_skill_entity});

        auto virtue_of_courage_skill_entity = registry.create();
        registry.emplace<component::is_skill>(virtue_of_courage_skill_entity);
        registry.ctx().emplace_hint<std::string>(virtue_of_courage_skill_entity,
                                                 "Virtue of Courage");
        registry.emplace<component::owner_actor>(virtue_of_courage_skill_entity,
                                                 component::owner_actor{actor_entity});
        skills_component.skill_entities.emplace_back(component::skill_entity{
            skill_database.find_by(actor::skill_t{base_class, "Virtue of Courage"}).skill_key,
            virtue_of_courage_skill_entity});
    }

    auto& available_weapons = registry.emplace<component::equipped_weapons>(
        actor_entity, component::equipped_weapons{actor_build.available_weapon_configurations});
    auto& current_weapon_set = registry.emplace<component::current_weapon_set>(actor_entity);
    for (auto& equipped_weapon : available_weapons.weapons) {
        if (equipped_weapon.set != current_weapon_set.set) {
            continue;
        }

        auto this_weapon_skills =
            skill_database.find_by(equipped_weapon.type, equipped_weapon.position);
        for (auto& weapon_skill : this_weapon_skills) {
            if (weapon_skill.is_child_skill) {
                continue;
            }

            for (int i = 0; i < weapon_skill.ammo; ++i) {
                auto skill_entity = registry.create();
                registry.emplace<component::is_skill>(skill_entity);
                registry.ctx().emplace_hint<std::string>(skill_entity, weapon_skill.skill_key.name);
                registry.emplace<component::owner_actor>(skill_entity,
                                                         component::owner_actor{actor_entity});

                if (!weapon_skill.attribute_modifications.empty()) {
                    auto& attribute_modifications_component =
                        registry.emplace<component::attribute_modifications_component>(
                            skill_entity);
                    std::copy(weapon_skill.attribute_modifications.cbegin(),
                              weapon_skill.attribute_modifications.cend(),
                              std::back_inserter(
                                  attribute_modifications_component.attribute_modifications));
                }

                skills_component.skill_entities.emplace_back(
                    component::skill_entity{weapon_skill.skill_key, skill_entity});
            }
        }
    }
    for (auto& slot_skill_name : actor_build.equipped_slot_skill_names) {
        auto& slot_skill = skill_database.find_by(actor::skill_t{base_class, slot_skill_name});
        auto skill_entity = registry.create();
        registry.emplace<component::is_skill>(skill_entity);
        registry.ctx().emplace_hint<std::string>(skill_entity, slot_skill.skill_key.name);
        registry.emplace<component::owner_actor>(skill_entity,
                                                 component::owner_actor{actor_entity});
        if (!slot_skill.attribute_modifications.empty()) {
            auto& attribute_modifications_component =
                registry.emplace<component::attribute_modifications_component>(skill_entity);
            std::copy(
                slot_skill.attribute_modifications.cbegin(),
                slot_skill.attribute_modifications.cend(),
                std::back_inserter(attribute_modifications_component.attribute_modifications));
        }
        skills_component.skill_entities.emplace_back(
            component::skill_entity{slot_skill.skill_key, skill_entity});
    }

    if (provided_rotation_ptr == nullptr) {
        auto actor_rotation = actor::rotation_t::read(
            fmt::format("rotation-{}.csv", configuration_suffix), base_class);
        registry.emplace<component::rotation_component>(
            actor_entity, component::rotation_component{actor_rotation, 0, 0, false});
    } else {
        registry.emplace<component::rotation_component>(
            actor_entity, component::rotation_component{*provided_rotation_ptr, 0, 0, false});
    }

    registry.emplace<component::static_attributes>(
        actor_entity, utils::get_static_attributes_from_build(actor_build));

    registry.emplace<component::team>(actor_entity, component::team{team_id});
}

void build_actors(registry_t& registry) {
    build_actor(registry, "cfb", 1);

    actor::rotation_t technician_rotation;
    technician_rotation.skill_casts.emplace_back(actor::skill_cast_t{
        actor::skill_t{actor::base_class_t::UNIVERSAL, "Pulse Boons and Conditions"}, 0});
    build_actor(registry, "technician", 1, &technician_rotation);

    actor::rotation_t golem_rotation;
    build_actor(registry, "golem", 2, &golem_rotation);
}

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::effective_attributes, component::finished_casting_skill>();
}

void do_tick(registry_t& registry) {
    system::progress_cooldowns(registry);
    system::calculate_effective_attributes(registry);
    // utils::log_component<component::effective_attributes>(registry);

    system::continue_rotation(registry);
    // utils::log_component<component::animation>(registry);

    system::do_animation(registry);
    utils::log_component<component::animation>(registry);
    utils::log_component<component::finished_casting_skill>(registry);

    clear_temporary_components(registry);
}

void combat_loop() {
    registry_t registry;
    build_actors(registry);

    tick_t current_tick{0};
    registry.ctx().emplace<const tick_t&>(current_tick);

    while (current_tick < 30001) {
        do_tick(registry);
        ++current_tick;
    }
}

}  // namespace gw2combat

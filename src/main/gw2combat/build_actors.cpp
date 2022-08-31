#include "init.hpp"

#include "gw2combat/utilities/actor_utilities.hpp"

#include "actor/build.hpp"
#include "actor/rotation.hpp"
#include "actor/skill.hpp"
#include "actor/skill_database.hpp"

#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/is_actor.hpp"
#include "gw2combat/component/actor/rotation_component.hpp"
#include "gw2combat/component/actor/rune_component.hpp"
#include "gw2combat/component/actor/static_attributes.hpp"
#include "gw2combat/component/actor/team.hpp"
#include "gw2combat/component/effect/duration.hpp"
#include "gw2combat/component/equipment/weapons.hpp"
#include "gw2combat/component/trait/is_trait.hpp"

namespace gw2combat {

auto build_actor(registry_t& registry,
                 const std::string& configuration_suffix,
                 int team_id,
                 actor::rotation_t* provided_rotation_ptr = nullptr,
                 bool repeat_rotation = false) {
    auto actor_build = actor::build_t::read(fmt::format("build-{}.json", configuration_suffix));
    auto base_class = actor_build.base_class;

    auto actor_entity = registry.create();
    registry.ctx().emplace_hint<std::string>(
        actor_entity, fmt::format("actor{}-{}", actor_entity, configuration_suffix));
    registry.emplace<component::is_actor>(actor_entity);
    registry.emplace<component::effects_component>(actor_entity);

    registry.emplace<component::static_attributes>(
        actor_entity, utils::get_static_attributes_from_build(actor_build));
    registry.emplace<component::rune_component>(actor_entity,
                                                component::rune_component{actor_build.rune});

    for (auto& permanent_unique_effect : actor_build.permanent_unique_effects) {
        entity_t unique_effect_entity =
            *utils::add_unique_effect_to_actor(permanent_unique_effect, actor_entity, registry);
        registry.emplace<component::duration>(unique_effect_entity,
                                              component::duration{2'000'000'000});
    }

    registry.emplace<component::current_weapon_set>(actor_entity);
    auto& available_weapons = registry.emplace<component::equipped_weapons>(
        actor_entity, component::equipped_weapons{actor_build.available_weapon_configurations});

    registry.emplace<component::team>(actor_entity, component::team{team_id});

    for (const std::string& trait : actor_build.traits) {
        utils::add_trait_to_actor(actor::trait_t{base_class, trait}, actor_entity, registry);
    }

    auto& skill_database =
        registry.emplace<actor::skill_database>(actor_entity, actor::skill_database::copy());
    utils::add_skill_to_actor(actor::base_class_t::UNIVERSAL, "Idle_1ms", actor_entity, registry);
    utils::add_skill_to_actor(actor::base_class_t::UNIVERSAL, "Idle_10ms", actor_entity, registry);
    utils::add_skill_to_actor(actor::base_class_t::UNIVERSAL, "Idle_30s", actor_entity, registry);
    utils::add_skill_to_actor(
        actor::base_class_t::UNIVERSAL, "Weapon Swap", actor_entity, registry);
    utils::add_skill_to_actor(
        actor::base_class_t::UNIVERSAL, "Weapon Draw", actor_entity, registry);

    if (base_class == actor::base_class_t::GUARDIAN) {
        utils::add_unique_effect_to_actor(
            actor::unique_effect_t{"Spear of Justice Passive Effect"}, actor_entity, registry);
    }

    bool elite_specialization_selected = false;
    if (!actor_build.trait_lines.empty()) {
        if (std::find(actor_build.trait_lines.begin(),
                      actor_build.trait_lines.end(),
                      actor::trait_line_t::FIREBRAND) != actor_build.trait_lines.end()) {
            elite_specialization_selected = true;

            utils::add_skill_to_actor(base_class, "Tome of Justice", actor_entity, registry);
            auto tome_skills = skill_database.find_by(actor::skill_tag_t::TOME);
            for (auto& tome_skill : tome_skills) {
                utils::add_skill_to_actor(
                    base_class, tome_skill.skill_key.name, actor_entity, registry);
            }
            if (std::find(actor_build.traits.begin(), actor_build.traits.end(), "Glacial Heart") !=
                actor_build.traits.end()) {
                utils::add_skill_to_actor(base_class, "Glacial Heart", actor_entity, registry);
            }
        } else if (std::find(actor_build.trait_lines.begin(),
                             actor_build.trait_lines.end(),
                             actor::trait_line_t::DRAGONHUNTER) != actor_build.trait_lines.end()) {
            utils::add_skill_to_actor(base_class, "Spear of Justice", actor_entity, registry);
        }
    }
    if (!elite_specialization_selected) {
        if (base_class == actor::base_class_t::GUARDIAN) {
            utils::add_skill_to_actor(base_class, "Virtue of Justice", actor_entity, registry);
            utils::add_skill_to_actor(base_class, "Virtue of Resolve", actor_entity, registry);
            utils::add_skill_to_actor(base_class, "Virtue of Courage", actor_entity, registry);
        }
    }

    for (auto& weapon : available_weapons.weapons) {
        auto this_weapon_skills = skill_database.find_by(weapon.type, weapon.position);
        for (auto& weapon_skill : this_weapon_skills) {
            if (weapon_skill.is_child_skill) {
                continue;
            }
            utils::add_skill_to_actor(weapon_skill.skill_key.base_class,
                                      weapon_skill.skill_key.name,
                                      actor_entity,
                                      registry);
        }
    }
    for (auto& slot_skill_name : actor_build.equipped_slot_skill_names) {
        auto& slot_skill = skill_database.find_by(actor::skill_t{base_class, slot_skill_name});
        utils::add_skill_to_actor(
            slot_skill.skill_key.base_class, slot_skill.skill_key.name, actor_entity, registry);
    }

    if (provided_rotation_ptr == nullptr) {
        auto actor_rotation = actor::rotation_t::read(
            fmt::format("rotation-{}.csv", configuration_suffix), base_class);
        registry.emplace<component::rotation_component>(
            actor_entity, component::rotation_component{actor_rotation, 0, 0, repeat_rotation});
    } else {
        registry.emplace<component::rotation_component>(
            actor_entity,
            component::rotation_component{*provided_rotation_ptr, 0, 0, repeat_rotation});
    }
    return actor_entity;
}

void build_actors(registry_t& registry) {
    build_actor(registry, "dh", 1);

    actor::rotation_t technician_rotation;
    technician_rotation.skill_casts.emplace_back(actor::skill_cast_t{
        actor::skill_t{actor::base_class_t::UNIVERSAL, "Pulse Boons and Conditions"}, 0});
    technician_rotation.skill_casts.emplace_back(
        actor::skill_cast_t{actor::skill_t{actor::base_class_t::UNIVERSAL, "Idle_30s"}, 0});
    build_actor(registry, "technician", 1, &technician_rotation, true);

    actor::rotation_t golem_rotation;
    build_actor(registry, "golem", 2, &golem_rotation);
}

}  // namespace gw2combat

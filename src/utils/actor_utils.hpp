#ifndef GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP
#define GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP

#include "common.hpp"

#include "io_utils.hpp"

#include "configuration/skill.hpp"
#include "configuration/unique_effect.hpp"

#include "component/hierarchy/owner_component.hpp"

namespace gw2combat::utils {

template <typename ConfigurationType, typename ComponentType>
static inline entity_t add_owner_based_component(const ConfigurationType& configuration_type_value,
                                                 entity_t parent_entity,
                                                 registry_t& registry) {
    auto component_type_holder_entity = registry.create();
    registry.ctx().emplace_as<std::string>(
        component_type_holder_entity,
        std::string{utils::get_component_name_with_prefix<ConfigurationType>()} + " holder entity");

    registry.emplace<component::owner_component>(component_type_holder_entity, parent_entity);
    registry.emplace<ComponentType>(component_type_holder_entity, configuration_type_value);
    return component_type_holder_entity;
}

[[nodiscard]] entity_t create_temporary_rotation_child_actor(entity_t parent_actor,
                                                             const std::string& name,
                                                             int team_id,
                                                             registry_t& registry);
entity_t add_skill_to_actor(const configuration::skill_t& skill,
                            entity_t actor_entity,
                            registry_t& registry);
void enqueue_child_skills(entity_t parent_actor,
                          const std::string& child_name,
                          const std::vector<actor::skill_t>& skills,
                          registry_t& registry);
void enqueue_child_skill(const actor::skill_t& skill, entity_t parent_actor, registry_t& registry);
entity_t add_effect_to_actor(actor::effect_t effect,
                             entity_t actor_entity,
                             entity_t source_actor,
                             const actor::skill_t& source_skill,
                             int duration,
                             registry_t& registry);
std::optional<entity_t> add_permanent_effect_to_actor(actor::effect_t effect,
                                                      entity_t actor_entity,
                                                      registry_t& registry);
std::vector<entity_t> add_effect_to_actor(actor::effect_t effect,
                                          int num_stacks,
                                          int duration,
                                          const actor::skill_t& source_skill,
                                          entity_t source_entity,
                                          entity_t target_entity,
                                          registry_t& registry);
std::optional<entity_t> add_unique_effect_to_actor(
    const configuration::unique_effect_t& unique_effect,
    entity_t actor_entity,
    entity_t source_actor,
    const actor::skill_t& source_skill,
    int duration,
    registry_t& registry);
std::optional<entity_t> add_permanent_unique_effect_to_actor(
    const configuration::unique_effect_t& unique_effect,
    entity_t actor_entity,
    registry_t& registry);
std::vector<entity_t> add_unique_effect_to_actor(
    const configuration::unique_effect_t& unique_effect,
    int num_stacks,
    int duration,
    const actor::skill_t& source_skill,
    entity_t source_entity,
    entity_t target_entity,
    registry_t& registry);
void finish_casting_skill(entity_t actor_entity, entity_t skill_entity, registry_t& registry);

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP

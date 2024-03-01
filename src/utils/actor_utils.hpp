#ifndef GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP
#define GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP

#include "common.hpp"

#include "io_utils.hpp"
#include "skill_utils.hpp"

#include "configuration/cooldown_modifier.hpp"
#include "configuration/skill.hpp"
#include "configuration/unique_effect.hpp"

#include "component/hierarchy/owner_component.hpp"
#include "component/temporal/cooldown_component.hpp"

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
entity_t add_conditional_skill_group_to_actor(
    const configuration::conditional_skill_group_t& conditional_skill_group,
    entity_t actor_entity,
    registry_t& registry);
void enqueue_child_skills(entity_t parent_actor,
                          const std::string& child_name,
                          const std::vector<actor::skill_t>& skills,
                          registry_t& registry);
void enqueue_child_skill(const actor::skill_t& skill, entity_t parent_actor, registry_t& registry);
void enqueue_source_actor_child_skill(const actor::skill_t& skill,
                                      entity_t source_actor,
                                      registry_t& registry);
entity_t add_effect_to_actor(actor::effect_t effect,
                             entity_t actor_entity,
                             entity_t source_actor,
                             const actor::skill_t& source_skill,
                             int duration,
                             int grouped_with_num_stacks,
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
void finish_casting_skill(entity_t skill_entity, entity_t actor_entity, registry_t& registry);

static inline void apply_cooldown_modifications(
    registry_t& registry,
    entity_t actor_entity,
    const configuration::cooldown_modifier_t& cooldown_modifier) {
    auto skill_entity =
        utils::get_skill_entity(cooldown_modifier.skill_key, actor_entity, registry);
    auto cooldown_ptr = registry.try_get<component::cooldown_component>(skill_entity);
    if (!cooldown_ptr) {
        return;
    }
    auto operation_fn = [&](int modifier) {
        switch (cooldown_modifier.operation) {
            case configuration::cooldown_modifier_t::operation_t::ADD:
                cooldown_ptr->progress[0] -= modifier;
                break;
            case configuration::cooldown_modifier_t::operation_t::SUBTRACT:
                cooldown_ptr->progress[0] += modifier;
                break;
            case configuration::cooldown_modifier_t::operation_t::SET:
                cooldown_ptr->progress[0] = modifier;
                cooldown_ptr->progress[1] = 0;
                break;
            default:
                break;
        }
    };
    if (cooldown_modifier.operation == configuration::cooldown_modifier_t::operation_t::RESET) {
        cooldown_ptr->progress[0] = cooldown_ptr->duration[0];
        cooldown_ptr->progress[1] = 0;
    } else {
        operation_fn(cooldown_modifier.value);
    }
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP

#ifndef GW2COMBAT_UTILS_CONDITION_UTILS_HPP
#define GW2COMBAT_UTILS_CONDITION_UTILS_HPP

#include "common.hpp"

#include "configuration/condition.hpp"
#include "configuration/skill.hpp"

#include "component/damage/strikes_pipeline.hpp"

#include "basic_utils.hpp"

namespace gw2combat::utils {

struct condition_result_t {
    bool satisfied;
    std::string reason;
};

[[nodiscard]] extern condition_result_t independent_conditions_satisfied(
    const configuration::condition_t& condition,
    entity_t entity,
    std::optional<entity_t> target_entity,
    registry_t& registry);
[[nodiscard]] extern bool on_begun_casting_conditions_satisfied(
    const configuration::condition_t& condition,
    entity_t entity,
    const configuration::skill_t& source_skill_configuration,
    registry_t& registry);
[[nodiscard]] extern bool on_finished_casting_conditions_satisfied(
    const configuration::condition_t& condition,
    entity_t entity,
    const configuration::skill_t& source_skill_configuration,
    registry_t& registry);
[[nodiscard]] extern bool on_strike_conditions_satisfied(
    const configuration::condition_t& condition,
    entity_t entity,
    entity_t target_entity,
    bool is_critical,
    const component::strike_t& strike,
    const configuration::skill_t& source_skill_configuration,
    registry_t& registry);
[[nodiscard]] bool on_effect_application_conditions_satisfied(
    const configuration::condition_t& condition,
    entity_t entity,
    entity_t target_entity,
    actor::effect_t effect,
    registry_t& registry);
[[nodiscard]] bool on_ammo_gain_conditions_satisfied(
    const configuration::condition_t& condition,
    entity_t entity,
    const configuration::skill_t& source_skill_configuration,
    registry_t& registry);

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_CONDITION_UTILS_HPP

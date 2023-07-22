#ifndef GW2COMBAT_UTILS_SKILL_UTILS_HPP
#define GW2COMBAT_UTILS_SKILL_UTILS_HPP

#include "actor/skill.hpp"

#include "configuration/skill.hpp"

namespace gw2combat::utils {

struct skill_castability_t {
    bool can_cast;
    std::string reason;
};

[[nodiscard]] extern skill_castability_t can_cast_skill(const actor::skill_t& skill,
                                                        entity_t actor_entity,
                                                        registry_t& registry);
[[nodiscard]] extern entity_t get_skill_entity(const actor::skill_t& skill,
                                               entity_t actor_entity,
                                               registry_t& registry);
[[nodiscard]] extern configuration::skill_t& get_skill_configuration(const actor::skill_t& skill,
                                                                     entity_t actor_entity,
                                                                     registry_t& registry);
[[nodiscard]] extern bool skill_has_tag(const configuration::skill_t& skill,
                                        const actor::skill_tag_t& skill_tag);
extern void put_skill_on_cooldown(const actor::skill_t& skill,
                                  entity_t actor_entity,
                                  registry_t& registry,
                                  bool force = false);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(skill_castability_t, can_cast, reason)

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_SKILL_UTILS_HPP

#ifndef GW2COMBAT_CONFIGURATION_BUILD_HPP
#define GW2COMBAT_CONFIGURATION_BUILD_HPP

#include "common.hpp"

#include "actor/attributes.hpp"
#include "actor/base_class.hpp"
#include "actor/effect.hpp"
#include "actor/profession.hpp"
#include "actor/weapon.hpp"

#include "unique_effect.hpp"
#include "weapon.hpp"
#include "skill.hpp"

namespace gw2combat::configuration {

struct build_t {
    actor::base_class_t base_class = actor::base_class_t::INVALID;
    actor::profession_t profession = actor::profession_t::INVALID;
    std::unordered_map<actor::attribute_t, double> attributes{};
    std::vector<weapon> weapons{};
    std::vector<skill_t> skills{};
    std::vector<actor::effect_t> permanent_effects{};
    std::vector<unique_effect_t> permanent_unique_effects{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(build_t,
                                                base_class,
                                                profession,
                                                attributes,
                                                weapons,
                                                skills,
                                                permanent_effects,
                                                permanent_unique_effects)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_BUILD_HPP

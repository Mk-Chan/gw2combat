#ifndef GW2COMBAT_CONFIGURATION_COOLDOWN_MODIFIER_HPP
#define GW2COMBAT_CONFIGURATION_COOLDOWN_MODIFIER_HPP

#include "common.hpp"

#include "condition.hpp"

namespace gw2combat::configuration {

struct cooldown_modifier_t {
    enum class operation_t
    {
        INVALID,

        ADD,
        SUBTRACT,
        SET,
        RESET,
    };

    condition_t condition{};

    actor::skill_t skill_key;
    operation_t operation = operation_t::ADD;
    int value = 0;
};

NLOHMANN_JSON_SERIALIZE_ENUM(cooldown_modifier_t::operation_t,
                             {
                                 {cooldown_modifier_t::operation_t::INVALID, "invalid"},
                                 {cooldown_modifier_t::operation_t::ADD, "add"},
                                 {cooldown_modifier_t::operation_t::SUBTRACT, "subtract"},
                                 {cooldown_modifier_t::operation_t::SET, "set"},
                                 {cooldown_modifier_t::operation_t::RESET, "reset"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(cooldown_modifier_t,
                                                condition,
                                                skill_key,
                                                operation,
                                                value)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_COOLDOWN_MODIFIER_HPP

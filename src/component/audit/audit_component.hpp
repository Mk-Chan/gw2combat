#ifndef GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP

#include "common.hpp"

#include <variant>

#include "component/damage/incoming_damage.hpp"

namespace gw2combat::component {

enum class audit_event_type_t
{
    DAMAGE_EVENT,
    SKILL_CAST_BEGIN_EVENT,
    SKILL_CAST_END_EVENT,
};

struct audit_skill_cast_begin_event_t {
    audit_event_type_t event_type = audit_event_type_t::SKILL_CAST_BEGIN_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string skill;
};

struct audit_skill_cast_end_event_t {
    audit_event_type_t event_type = audit_event_type_t::SKILL_CAST_END_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string skill;
};

struct audit_damage_event_t {
    enum class damage_type_t
    {
        INVALID,

        STRIKE,
        BINDING_BLADE,
        BLEEDING,
        BURNING,
        CONFUSION,
        CONFUSION_ON_SKILL_ACTIVATION,
        POISON,
        TORMENT_STATIONARY,
        TORMENT_MOVING,
    };

    audit_event_type_t event_type = audit_event_type_t::DAMAGE_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string source_skill;
    damage_type_t damage_type = damage_type_t::INVALID;
    int damage = 0;
};

struct audit_component {
    std::string audit_base_path;
    std::vector<std::variant<component::audit_damage_event_t,
                             component::audit_skill_cast_begin_event_t,
                             component::audit_skill_cast_end_event_t>>
        events;
};

NLOHMANN_JSON_SERIALIZE_ENUM(audit_event_type_t,
                             {
                                 {audit_event_type_t::DAMAGE_EVENT, "damage_event"},
                                 {audit_event_type_t::SKILL_CAST_BEGIN_EVENT,
                                  "skill_cast_begin_event"},
                                 {audit_event_type_t::SKILL_CAST_END_EVENT, "skill_cast_end_event"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(
    audit_damage_event_t::damage_type_t,
    {
        {audit_damage_event_t::damage_type_t::INVALID, "invalid"},
        {audit_damage_event_t::damage_type_t::STRIKE, "strike"},
        {audit_damage_event_t::damage_type_t::BINDING_BLADE, "binding_blade"},
        {audit_damage_event_t::damage_type_t::BLEEDING, "bleeding"},
        {audit_damage_event_t::damage_type_t::BURNING, "burning"},
        {audit_damage_event_t::damage_type_t::CONFUSION, "confusion"},
        {audit_damage_event_t::damage_type_t::CONFUSION_ON_SKILL_ACTIVATION,
         "confusion_on_skill_activation"},
        {audit_damage_event_t::damage_type_t::POISON, "poison"},
        {audit_damage_event_t::damage_type_t::TORMENT_STATIONARY, "torment_stationary"},
        {audit_damage_event_t::damage_type_t::TORMENT_MOVING, "torment_moving"},
    })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(audit_damage_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                source_skill,
                                                damage_type,
                                                damage)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(audit_skill_cast_begin_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                skill)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(audit_skill_cast_end_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                skill)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP

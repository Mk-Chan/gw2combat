#ifndef GW2COMBAT_AUDIT_EVENTS_HPP
#define GW2COMBAT_AUDIT_EVENTS_HPP

#include "common.hpp"

#include "actor/effect.hpp"

namespace gw2combat::audit {

enum class event_type_t
{
    SKILL_CAST_BEGIN_EVENT,
    SKILL_CAST_END_EVENT,
    EFFECT_APPLICATION_EVENT,
    DAMAGE_EVENT,
    COMBAT_STATS_UPDATE_EVENT,
};

struct skill_cast_begin_event_t {
    event_type_t event_type = event_type_t::SKILL_CAST_BEGIN_EVENT;
    tick_t time_ms = 0;
    std::string skill;
};

struct skill_cast_end_event_t {
    event_type_t event_type = event_type_t::SKILL_CAST_END_EVENT;
    tick_t time_ms = 0;
    std::string skill;
};

struct effect_application_event_t {
    event_type_t event_type = event_type_t::EFFECT_APPLICATION_EVENT;
    tick_t time_ms = 0;
    std::string source_actor;
    std::string source_skill;
    std::string effect;
    std::string unique_effect;
    int num_stacks = 0;
    int duration_ms = 0;
};

struct damage_event_t {
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

    event_type_t event_type = event_type_t::DAMAGE_EVENT;
    tick_t time_ms = 0;
    std::string source_actor;
    std::string source_skill;
    damage_type_t damage_type = damage_type_t::INVALID;
    int damage = 0;
};

struct combat_stats_update_event_t {
    event_type_t event_type = event_type_t::COMBAT_STATS_UPDATE_EVENT;
    tick_t time_ms = 0;
    int updated_health = 0;
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    event_type_t,
    {
        {event_type_t::SKILL_CAST_BEGIN_EVENT, "skill_cast_begin_event"},
        {event_type_t::SKILL_CAST_END_EVENT, "skill_cast_end_event"},
        {event_type_t::EFFECT_APPLICATION_EVENT, "effect_application_event"},
        {event_type_t::DAMAGE_EVENT, "damage_event"},
        {event_type_t::COMBAT_STATS_UPDATE_EVENT, "combat_stats_update_event"},
    })
NLOHMANN_JSON_SERIALIZE_ENUM(damage_event_t::damage_type_t,
                             {
                                 {damage_event_t::damage_type_t::INVALID, "invalid"},
                                 {damage_event_t::damage_type_t::STRIKE, "strike"},
                                 {damage_event_t::damage_type_t::BINDING_BLADE, "binding_blade"},
                                 {damage_event_t::damage_type_t::BLEEDING, "bleeding"},
                                 {damage_event_t::damage_type_t::BURNING, "burning"},
                                 {damage_event_t::damage_type_t::CONFUSION, "confusion"},
                                 {damage_event_t::damage_type_t::CONFUSION_ON_SKILL_ACTIVATION,
                                  "confusion_on_skill_activation"},
                                 {damage_event_t::damage_type_t::POISON, "poison"},
                                 {damage_event_t::damage_type_t::TORMENT_STATIONARY,
                                  "torment_stationary"},
                                 {damage_event_t::damage_type_t::TORMENT_MOVING, "torment_moving"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(damage_event_t,
                                                event_type,
                                                time_ms,
                                                source_actor,
                                                source_skill,
                                                damage_type,
                                                damage)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_cast_begin_event_t,
                                                event_type,
                                                time_ms,
                                                skill)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_cast_end_event_t, event_type, time_ms, skill)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(effect_application_event_t,
                                                event_type,
                                                time_ms,
                                                source_actor,
                                                source_skill,
                                                effect,
                                                unique_effect,
                                                num_stacks,
                                                duration_ms)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(combat_stats_update_event_t,
                                                event_type,
                                                time_ms,
                                                updated_health)

}  // namespace gw2combat::audit

#endif  // GW2COMBAT_AUDIT_EVENTS_HPP

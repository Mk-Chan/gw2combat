#ifndef GW2COMBAT_AUDIT_EVENTS_HPP
#define GW2COMBAT_AUDIT_EVENTS_HPP

#include "common.hpp"

#include "actor/effect.hpp"

namespace gw2combat::audit {

enum class event_type_t
{
    INVALID,

    ACTOR_CREATED_EVENT,
    SKILL_CAST_BEGIN_EVENT,
    SKILL_CAST_END_EVENT,
    EQUIPPED_BUNDLE_EVENT,
    DROPPED_BUNDLE_EVENT,
    EFFECT_APPLICATION_EVENT,
    DAMAGE_EVENT,
    COMBAT_STATS_UPDATE_EVENT,
    EFFECT_EXPIRED_EVENT,
    ACTOR_DOWNSTATE_EVENT,
};

struct actor_created_event_t {
    event_type_t event_type = event_type_t::ACTOR_CREATED_EVENT;
};

struct skill_cast_begin_event_t {
    event_type_t event_type = event_type_t::SKILL_CAST_BEGIN_EVENT;
    std::string skill;
};

struct skill_cast_end_event_t {
    event_type_t event_type = event_type_t::SKILL_CAST_END_EVENT;
    std::string skill;
};

struct equipped_bundle_event_t {
    event_type_t event_type = event_type_t::EQUIPPED_BUNDLE_EVENT;
    std::string bundle;
};

struct dropped_bundle_event_t {
    event_type_t event_type = event_type_t::DROPPED_BUNDLE_EVENT;
    std::string bundle;
};

struct effect_application_event_t {
    event_type_t event_type = event_type_t::EFFECT_APPLICATION_EVENT;
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
    std::string source_actor;
    std::string source_skill;
    damage_type_t damage_type = damage_type_t::INVALID;
    int damage = 0;
};

struct combat_stats_update_event_t {
    event_type_t event_type = event_type_t::COMBAT_STATS_UPDATE_EVENT;
    int updated_health = 0;
};

struct effect_expired_event_t {
    event_type_t event_type = event_type_t::EFFECT_EXPIRED_EVENT;
    std::string source_actor;
    std::string source_skill;
    std::string effect;
    std::string unique_effect;
};

struct actor_downstate_event_t {
    event_type_t event_type = event_type_t::ACTOR_DOWNSTATE_EVENT;
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    event_type_t,
    {
        {event_type_t::INVALID, "invalid"},
        {event_type_t::ACTOR_CREATED_EVENT, "actor_created_event"},
        {event_type_t::SKILL_CAST_BEGIN_EVENT, "skill_cast_begin_event"},
        {event_type_t::SKILL_CAST_END_EVENT, "skill_cast_end_event"},
        {event_type_t::EQUIPPED_BUNDLE_EVENT, "equipped_bundle_event"},
        {event_type_t::DROPPED_BUNDLE_EVENT, "dropped_bundle_event"},
        {event_type_t::EFFECT_APPLICATION_EVENT, "effect_application_event"},
        {event_type_t::DAMAGE_EVENT, "damage_event"},
        {event_type_t::COMBAT_STATS_UPDATE_EVENT, "combat_stats_update_event"},
        {event_type_t::EFFECT_EXPIRED_EVENT, "effect_expired_event"},
        {event_type_t::ACTOR_DOWNSTATE_EVENT, "actor_downstate_event"},
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
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_created_event_t, event_type)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(damage_event_t,
                                                event_type,
                                                source_actor,
                                                source_skill,
                                                damage_type,
                                                damage)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_cast_begin_event_t, event_type, skill)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_cast_end_event_t, event_type, skill)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(equipped_bundle_event_t, event_type, bundle)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(dropped_bundle_event_t, event_type, bundle)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(effect_application_event_t,
                                                event_type,
                                                source_actor,
                                                source_skill,
                                                effect,
                                                unique_effect,
                                                num_stacks,
                                                duration_ms)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(combat_stats_update_event_t,
                                                event_type,
                                                updated_health)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(effect_expired_event_t,
                                                event_type,
                                                source_actor,
                                                source_skill,
                                                effect,
                                                unique_effect)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_downstate_event_t, event_type)

}  // namespace gw2combat::audit

#endif  // GW2COMBAT_AUDIT_EVENTS_HPP

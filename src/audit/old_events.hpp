#ifndef GW2COMBAT_AUDIT_OLD_EVENTS_HPP
#define GW2COMBAT_AUDIT_OLD_EVENTS_HPP

#include "common.hpp"

#include "actor/effect.hpp"

namespace gw2combat::audit {

enum class old_event_type_t
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

struct old_actor_created_event_t {
    old_event_type_t event_type = old_event_type_t::ACTOR_CREATED_EVENT;
    tick_t time_ms = 0;
    std::string actor;
};

struct old_skill_cast_begin_event_t {
    old_event_type_t event_type = old_event_type_t::SKILL_CAST_BEGIN_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string skill;
};

struct old_skill_cast_end_event_t {
    old_event_type_t event_type = old_event_type_t::SKILL_CAST_END_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string skill;
};

struct old_equipped_bundle_event_t {
    old_event_type_t event_type = old_event_type_t::EQUIPPED_BUNDLE_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string bundle;
};

struct old_dropped_bundle_event_t {
    old_event_type_t event_type = old_event_type_t::DROPPED_BUNDLE_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string bundle;
};

struct old_effect_application_event_t {
    old_event_type_t event_type = old_event_type_t::EFFECT_APPLICATION_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string source_actor;
    std::string source_skill;
    std::string effect;
    std::string unique_effect;
    int num_stacks = 0;
    int duration_ms = 0;
};

struct old_damage_event_t {
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

    old_event_type_t event_type = old_event_type_t::DAMAGE_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string source_actor;
    std::string source_skill;
    damage_type_t damage_type = damage_type_t::INVALID;
    int damage = 0;
};

struct old_combat_stats_update_event_t {
    old_event_type_t event_type = old_event_type_t::COMBAT_STATS_UPDATE_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    int updated_health = 0;
};

struct old_effect_expired_event_t {
    old_event_type_t event_type = old_event_type_t::EFFECT_EXPIRED_EVENT;
    tick_t time_ms = 0;
    std::string actor;
    std::string source_actor;
    std::string source_skill;
    std::string effect;
    std::string unique_effect;
};

struct old_actor_downstate_event_t {
    old_event_type_t event_type = old_event_type_t::ACTOR_DOWNSTATE_EVENT;
    tick_t time_ms = 0;
    std::string actor;
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    old_event_type_t,
    {
        {old_event_type_t::INVALID, "invalid"},
        {old_event_type_t::ACTOR_CREATED_EVENT, "actor_created_event"},
        {old_event_type_t::SKILL_CAST_BEGIN_EVENT, "skill_cast_begin_event"},
        {old_event_type_t::SKILL_CAST_END_EVENT, "skill_cast_end_event"},
        {old_event_type_t::EQUIPPED_BUNDLE_EVENT, "equipped_bundle_event"},
        {old_event_type_t::DROPPED_BUNDLE_EVENT, "dropped_bundle_event"},
        {old_event_type_t::EFFECT_APPLICATION_EVENT, "effect_application_event"},
        {old_event_type_t::DAMAGE_EVENT, "damage_event"},
        {old_event_type_t::COMBAT_STATS_UPDATE_EVENT, "combat_stats_update_event"},
        {old_event_type_t::EFFECT_EXPIRED_EVENT, "effect_expired_event"},
        {old_event_type_t::ACTOR_DOWNSTATE_EVENT, "actor_downstate_event"},
    })
NLOHMANN_JSON_SERIALIZE_ENUM(
    old_damage_event_t::damage_type_t,
    {
        {old_damage_event_t::damage_type_t::INVALID, "invalid"},
        {old_damage_event_t::damage_type_t::STRIKE, "strike"},
        {old_damage_event_t::damage_type_t::BINDING_BLADE, "binding_blade"},
        {old_damage_event_t::damage_type_t::BLEEDING, "bleeding"},
        {old_damage_event_t::damage_type_t::BURNING, "burning"},
        {old_damage_event_t::damage_type_t::CONFUSION, "confusion"},
        {old_damage_event_t::damage_type_t::CONFUSION_ON_SKILL_ACTIVATION,
         "confusion_on_skill_activation"},
        {old_damage_event_t::damage_type_t::POISON, "poison"},
        {old_damage_event_t::damage_type_t::TORMENT_STATIONARY, "torment_stationary"},
        {old_damage_event_t::damage_type_t::TORMENT_MOVING, "torment_moving"},
    })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_actor_created_event_t,
                                                event_type,
                                                time_ms,
                                                actor)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_damage_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                source_actor,
                                                source_skill,
                                                damage_type,
                                                damage)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_skill_cast_begin_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                skill)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_skill_cast_end_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                skill)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_equipped_bundle_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                bundle)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_dropped_bundle_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                bundle)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_effect_application_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                source_actor,
                                                source_skill,
                                                effect,
                                                unique_effect,
                                                num_stacks,
                                                duration_ms)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_combat_stats_update_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                updated_health)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_effect_expired_event_t,
                                                event_type,
                                                time_ms,
                                                actor,
                                                source_actor,
                                                source_skill,
                                                effect,
                                                unique_effect)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(old_actor_downstate_event_t,
                                                event_type,
                                                time_ms,
                                                actor)

}  // namespace gw2combat::audit

#endif  // GW2COMBAT_AUDIT_OLD_EVENTS_HPP

#ifndef GW2COMBAT_CONFIGURATION_AUDIT_HPP
#define GW2COMBAT_CONFIGURATION_AUDIT_HPP

#include "common.hpp"

namespace gw2combat::configuration {

struct audit_t {
    enum class audit_type_t
    {
        INVALID,

        ACTOR_CREATED,
        SKILL_CASTS,
        BUNDLES,
        EFFECT_APPLICATIONS,
        DAMAGE,
        COMBAT_STATS,
        EFFECT_EXPIRATION,
        ACTOR_DOWNSTATE,
    };

    std::unordered_set<audit_type_t> audits_to_perform{
        audit_type_t::ACTOR_CREATED,
        audit_type_t::SKILL_CASTS,
        audit_type_t::BUNDLES,
        audit_type_t::EFFECT_APPLICATIONS,
        audit_type_t::DAMAGE,
        audit_type_t::COMBAT_STATS,
        audit_type_t::EFFECT_EXPIRATION,
        audit_type_t::ACTOR_DOWNSTATE,
    };
};

NLOHMANN_JSON_SERIALIZE_ENUM(audit_t::audit_type_t,
                             {
                                 {audit_t::audit_type_t::INVALID, "invalid"},
                                 {audit_t::audit_type_t::ACTOR_CREATED, "ACTOR_CREATED"},
                                 {audit_t::audit_type_t::SKILL_CASTS, "SKILL_CASTS"},
                                 {audit_t::audit_type_t::BUNDLES, "BUNDLES"},
                                 {audit_t::audit_type_t::EFFECT_APPLICATIONS,
                                  "EFFECT_APPLICATIONS"},
                                 {audit_t::audit_type_t::DAMAGE, "DAMAGE"},
                                 {audit_t::audit_type_t::COMBAT_STATS, "COMBAT_STATS"},
                                 {audit_t::audit_type_t::EFFECT_EXPIRATION, "EFFECT_EXPIRATION"},
                                 {audit_t::audit_type_t::ACTOR_DOWNSTATE, "ACTOR_DOWNSTATE"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(audit_t, audits_to_perform)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_AUDIT_HPP

#ifndef GW2COMBAT_COMPONENT_DAMAGE_EFFECTS_PIPELINE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_EFFECTS_PIPELINE_HPP

#include "actor/effect.hpp"
#include "actor/unique_effect.hpp"

#include "configuration/condition.hpp"
#include "configuration/effect_application.hpp"
#include "configuration/unique_effect.hpp"

namespace gw2combat::component {

struct effect_application_t {
    enum class direction_t : std::uint8_t
    {
        INVALID,

        SELF,
        TEAM,
        OUTGOING,
    };

    configuration::condition_t condition;

    actor::skill_t source_skill;
    actor::effect_t effect = actor::effect_t::INVALID;
    configuration::unique_effect_t unique_effect;
    direction_t direction = direction_t::INVALID;
    int base_duration_ms = 0;
    int num_stacks = 1;
    int num_targets = 1;

    [[nodiscard]] static inline component::effect_application_t::direction_t convert_direction(
        const configuration::direction_t& direction) {
        switch (direction) {
            case configuration::direction_t::INVALID:
                return component::effect_application_t::direction_t::INVALID;
            case configuration::direction_t::SELF:
                return component::effect_application_t::direction_t::SELF;
            case configuration::direction_t::TEAM:
                return component::effect_application_t::direction_t::TEAM;
            case configuration::direction_t::OUTGOING:
                return component::effect_application_t::direction_t::OUTGOING;
        }
    }
};

struct outgoing_effect_application {
    entity_t source_entity = entt::tombstone;
    effect_application_t effect_application;
};
struct outgoing_effects_component {
    std::vector<effect_application_t> effect_applications;
};

struct incoming_effect_application {
    entity_t source_entity = entt::tombstone;
    effect_application_t effect_application;
};
struct incoming_effects_component {
    std::vector<incoming_effect_application> effect_applications;
};

NLOHMANN_JSON_SERIALIZE_ENUM(effect_application_t::direction_t,
                             {
                                 {effect_application_t::direction_t::INVALID, "Invalid"},

                                 {effect_application_t::direction_t::SELF, "SELF"},
                                 {effect_application_t::direction_t::TEAM, "TEAM"},
                                 {effect_application_t::direction_t::OUTGOING, "OUTGOING"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(effect_application_t,
                                                condition,
                                                effect,
                                                unique_effect,
                                                direction,
                                                base_duration_ms,
                                                num_stacks,
                                                num_targets)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(outgoing_effect_application,
                                                source_entity,
                                                effect_application)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_effect_application,
                                                source_entity,
                                                effect_application)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(outgoing_effects_component, effect_applications)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_effects_component, effect_applications)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_EFFECTS_PIPELINE_HPP

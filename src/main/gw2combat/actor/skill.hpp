#ifndef GW2COMBAT_SKILL_HPP
#define GW2COMBAT_SKILL_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/actor/base_class.hpp"

namespace gw2combat::actor {

enum class skill_tag_t : std::uint32_t
{
    INVALID,

    CANNOT_CRITICAL_STRIKE,
    CONSECRATION_ATTACK,
    CONSECRATION_CAST,
    TRAP,
    MANTRA,
    SYMBOL,
    SPIRIT_WEAPON,
    GUARDIAN_VIRTUE,
    TOME,
};

struct skill_t {
    base_class_t base_class = base_class_t::INVALID;
    std::string name;

    [[nodiscard]] constexpr inline bool operator==(const skill_t& rhs) const {
        return (this->base_class == base_class_t::UNIVERSAL ||
                rhs.base_class == base_class_t::UNIVERSAL || this->base_class == rhs.base_class) &&
               this->name == rhs.name;
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_t, base_class, name)
NLOHMANN_JSON_SERIALIZE_ENUM(skill_tag_t,
                             {
                                 {skill_tag_t::INVALID, "Invalid"},

                                 {skill_tag_t::CANNOT_CRITICAL_STRIKE, "CANNOT_CRITICAL_STRIKE"},
                                 {skill_tag_t::CONSECRATION_ATTACK, "CONSECRATION_ATTACK"},
                                 {skill_tag_t::CONSECRATION_CAST, "CONSECRATION_CAST"},
                                 {skill_tag_t::TRAP, "TRAP"},
                                 {skill_tag_t::MANTRA, "MANTRA"},
                                 {skill_tag_t::SYMBOL, "SYMBOL"},
                                 {skill_tag_t::SPIRIT_WEAPON, "SPIRIT_WEAPON"},
                                 {skill_tag_t::GUARDIAN_VIRTUE, "GUARDIAN_VIRTUE"},
                                 {skill_tag_t::TOME, "TOME"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_SKILL_HPP

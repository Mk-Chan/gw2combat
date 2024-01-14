#ifndef GW2COMBAT_ACTOR_SKILL_HPP
#define GW2COMBAT_ACTOR_SKILL_HPP

#include "common.hpp"

namespace gw2combat::actor {

using skill_t = std::string;
using skill_tag_t = std::string;

enum class combo_field_t
{
    INVALID,
    DARK,
    ETHEREAL,
    FIRE,
    ICE,
    LIGHT,
    LIGHTNING,
    POISON,
    SMOKE,
    WATER,
};

NLOHMANN_JSON_SERIALIZE_ENUM(combo_field_t,
                             {
                                 {combo_field_t::INVALID, "invalid"},
                                 {combo_field_t::DARK, "dark"},
                                 {combo_field_t::DARK, "DARK"},
                                 {combo_field_t::ETHEREAL, "ethereal"},
                                 {combo_field_t::ETHEREAL, "ETHEREAL"},
                                 {combo_field_t::FIRE, "fire"},
                                 {combo_field_t::FIRE, "FIRE"},
                                 {combo_field_t::ICE, "ice"},
                                 {combo_field_t::ICE, "ICE"},
                                 {combo_field_t::LIGHT, "light"},
                                 {combo_field_t::LIGHT, "LIGHT"},
                                 {combo_field_t::LIGHTNING, "lightning"},
                                 {combo_field_t::LIGHTNING, "LIGHTNING"},
                                 {combo_field_t::POISON, "poison"},
                                 {combo_field_t::POISON, "POISON"},
                                 {combo_field_t::SMOKE, "smoke"},
                                 {combo_field_t::SMOKE, "SMOKE"},
                                 {combo_field_t::WATER, "water"},
                                 {combo_field_t::WATER, "WATER"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_SKILL_HPP

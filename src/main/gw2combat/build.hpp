#ifndef GW2COMBAT_SRC_MAIN_GW2COMBAT_BUILD_HPP
#define GW2COMBAT_SRC_MAIN_GW2COMBAT_BUILD_HPP

#include "gw2combat/types.hpp"

#include "gw2combat/component/character/enhancement.hpp"
#include "gw2combat/component/character/static_attributes.hpp"
#include "gw2combat/component/gear/runes.hpp"
#include "gw2combat/component/gear/weapon_configurations.hpp"
#include "gw2combat/component/traits_component.hpp"

namespace gw2combat {

enum class base_class_type : std::uint8_t
{
    GUARDIAN
};

struct attributes_configuration {
    unsigned int power;
    unsigned int precision;
    unsigned int toughness;
    unsigned int vitality;
    unsigned int concentration;
    unsigned int condition_damage;
    unsigned int expertise;
    unsigned int ferocity;
    unsigned int healing_power;
    unsigned int armor;
    double boon_duration_pct;
    double critical_chance_pct;
    double critical_damage_pct;
    double condition_duration_pct;
    double burning_duration_pct;
    double bleeding_duration_pct;
    double confusion_duration_pct;
    double poison_duration_pct;
    double torment_duration_pct;
    unsigned int max_health;
};

struct available_weapon_configurations {
    std::vector<weapon_configuration> weapon_configurations;
};

struct character_build {
    [[nodiscard]] static character_build read(const std::string& path);

    base_class_type base_class;
    attributes_configuration attributes;
    std::vector<trait_line_type> trait_lines;
    std::vector<trait_type> traits;
    available_weapon_configurations available_weapon_configurations;
    std::vector<std::string> equipped_slot_skill_names;
    rune_type rune;
    component::enhancement_type enhancement;
};


NLOHMANN_JSON_SERIALIZE_ENUM(base_class_type,
                             {
                                 {base_class_type::GUARDIAN, "guardian"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(available_weapon_configurations, weapon_configurations)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(attributes_configuration,
                                   power,
                                   precision,
                                   toughness,
                                   vitality,
                                   concentration,
                                   condition_damage,
                                   expertise,
                                   ferocity,
                                   healing_power,
                                   armor,
                                   boon_duration_pct,
                                   critical_chance_pct,
                                   critical_damage_pct,
                                   condition_duration_pct,
                                   burning_duration_pct,
                                   bleeding_duration_pct,
                                   confusion_duration_pct,
                                   poison_duration_pct,
                                   torment_duration_pct,
                                   max_health)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(character_build,
                                   base_class,
                                   attributes,
                                   trait_lines,
                                   traits,
                                   available_weapon_configurations,
                                   equipped_slot_skill_names,
                                   rune,
                                   enhancement)

}  // namespace gw2combat

#endif  // GW2COMBAT_SRC_MAIN_GW2COMBAT_BUILD_HPP

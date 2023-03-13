#ifndef GW2COMBAT_CONFIGURATION_EFFECT_REMOVAL_HPP
#define GW2COMBAT_CONFIGURATION_EFFECT_REMOVAL_HPP

#include "actor/effect.hpp"
#include "actor/unique_effect.hpp"

#include "condition.hpp"

namespace gw2combat::configuration {

struct effect_removal_t {
    condition_t condition;

    actor::effect_t effect = actor::effect_t::INVALID;
    actor::unique_effect_t unique_effect;
    std::optional<int> num_stacks;
};

static inline void to_json(nlohmann::json& nlohmann_json_j,
                           const effect_removal_t& nlohmann_json_t) {
    nlohmann_json_j["condition"] = nlohmann_json_t.condition;
    nlohmann_json_j["effect"] = nlohmann_json_t.effect;
    nlohmann_json_j["unique_effect"] = nlohmann_json_t.unique_effect;
    if (nlohmann_json_t.num_stacks) {
        nlohmann_json_j["num_stacks"] = *nlohmann_json_t.num_stacks;
    }
}
static inline void from_json(const nlohmann::json& nlohmann_json_j,
                             effect_removal_t& nlohmann_json_t) {
    effect_removal_t nlohmann_json_default_obj;
    nlohmann_json_t.condition =
        nlohmann_json_j.value("condition", nlohmann_json_default_obj.condition);
    nlohmann_json_t.effect = nlohmann_json_j.value("effect", nlohmann_json_default_obj.effect);
    nlohmann_json_t.unique_effect =
        nlohmann_json_j.value("unique_effect", nlohmann_json_default_obj.unique_effect);
    if (nlohmann_json_j.contains("num_stacks")) {
        nlohmann_json_t.num_stacks =
            nlohmann_json_j.value("num_stacks", *nlohmann_json_default_obj.num_stacks);
    }
}

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_EFFECT_REMOVAL_HPP

#ifndef GW2COMBAT_CONFIGURATION_THRESHOLD_HPP
#define GW2COMBAT_CONFIGURATION_THRESHOLD_HPP

#include "common.hpp"

#include "actor/attributes.hpp"

#include "actor/counter.hpp"

namespace gw2combat::configuration {

struct threshold_t {
    // NOTE: Remember to add conditions to to_json and from_json if adding a member to this
    enum class type
    {
        INVALID,

        EQUAL,
        UPPER_BOUND_EXCLUSIVE,
        UPPER_BOUND_INCLUSIVE,
        LOWER_BOUND_EXCLUSIVE,
        LOWER_BOUND_INCLUSIVE,
    };
    type threshold_type = type::INVALID;
    double threshold_value = 0.0;

    std::optional<bool> generate_random_number_subject_to_threshold = std::nullopt;
    std::optional<bool> health_pct_subject_to_threshold = std::nullopt;
    std::optional<bool> target_health_pct_subject_to_threshold = std::nullopt;
    std::optional<actor::counter_t> counter_value_subject_to_threshold = std::nullopt;
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    threshold_t::type,
    {
        {threshold_t::type::INVALID, "invalid"},
        {threshold_t::type::EQUAL, "equal"},
        {threshold_t::type::UPPER_BOUND_EXCLUSIVE, "upper_bound_exclusive"},
        {threshold_t::type::UPPER_BOUND_INCLUSIVE, "upper_bound_inclusive"},
        {threshold_t::type::LOWER_BOUND_EXCLUSIVE, "lower_bound_exclusive"},
        {threshold_t::type::LOWER_BOUND_INCLUSIVE, "lower_bound_inclusive"},
    })

static inline void to_json(nlohmann::json& nlohmann_json_j, const threshold_t& nlohmann_json_t) {
    nlohmann_json_j["threshold_type"] = nlohmann_json_t.threshold_type;
    nlohmann_json_j["threshold_value"] = nlohmann_json_t.threshold_value;
    if (nlohmann_json_t.generate_random_number_subject_to_threshold) {
        nlohmann_json_j["generate_random_number_subject_to_threshold"] =
            *nlohmann_json_t.generate_random_number_subject_to_threshold;
    }
    if (nlohmann_json_t.health_pct_subject_to_threshold) {
        nlohmann_json_j["health_pct_subject_to_threshold"] =
            *nlohmann_json_t.health_pct_subject_to_threshold;
    }
    if (nlohmann_json_t.target_health_pct_subject_to_threshold) {
        nlohmann_json_j["target_health_pct_subject_to_threshold"] =
            *nlohmann_json_t.target_health_pct_subject_to_threshold;
    }
    if (nlohmann_json_t.counter_value_subject_to_threshold) {
        nlohmann_json_j["counter_value_subject_to_threshold"] =
            *nlohmann_json_t.counter_value_subject_to_threshold;
    }
}
static inline void from_json(const nlohmann::json& nlohmann_json_j, threshold_t& nlohmann_json_t) {
    threshold_t nlohmann_json_default_obj;
    nlohmann_json_t.threshold_type =
        nlohmann_json_j.value("threshold_type", nlohmann_json_default_obj.threshold_type);
    nlohmann_json_t.threshold_value =
        nlohmann_json_j.value("threshold_value", nlohmann_json_default_obj.threshold_value);
    if (nlohmann_json_j.contains("generate_random_number_subject_to_threshold")) {
        nlohmann_json_t.generate_random_number_subject_to_threshold = nlohmann_json_j.value(
            "generate_random_number_subject_to_threshold",
            *nlohmann_json_default_obj.generate_random_number_subject_to_threshold);
    }
    if (nlohmann_json_j.contains("health_pct_subject_to_threshold")) {
        nlohmann_json_t.health_pct_subject_to_threshold =
            nlohmann_json_j.value("health_pct_subject_to_threshold",
                                  *nlohmann_json_default_obj.health_pct_subject_to_threshold);
    }
    if (nlohmann_json_j.contains("target_health_pct_subject_to_threshold")) {
        nlohmann_json_t.target_health_pct_subject_to_threshold = nlohmann_json_j.value(
            "target_health_pct_subject_to_threshold",
            *nlohmann_json_default_obj.target_health_pct_subject_to_threshold);
    }
    if (nlohmann_json_j.contains("counter_value_subject_to_threshold")) {
        nlohmann_json_t.counter_value_subject_to_threshold =
            nlohmann_json_j.value("counter_value_subject_to_threshold",
                                  *nlohmann_json_default_obj.counter_value_subject_to_threshold);
    }
}

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_THRESHOLD_HPP

#ifndef GW2COMBAT_CONFIGURATION_COUNTER_MODIFIER_HPP
#define GW2COMBAT_CONFIGURATION_COUNTER_MODIFIER_HPP

#include "common.hpp"

#include "condition.hpp"

namespace gw2combat::configuration {

struct counter_modifier_t {
    enum class operation_t
    {
        INVALID,

        ADD,
        SUBTRACT,
        SET,
        RESET,
    };

    condition_t condition{};

    actor::counter_t counter_key;
    operation_t operation = operation_t::ADD;
    std::optional<int> value = std::nullopt;
    std::optional<actor::counter_t> counter_value = std::nullopt;
};

NLOHMANN_JSON_SERIALIZE_ENUM(counter_modifier_t::operation_t,
                             {
                                 {counter_modifier_t::operation_t::INVALID, "invalid"},
                                 {counter_modifier_t::operation_t::ADD, "add"},
                                 {counter_modifier_t::operation_t::ADD, "ADD"},
                                 {counter_modifier_t::operation_t::SUBTRACT, "subtract"},
                                 {counter_modifier_t::operation_t::SUBTRACT, "SUBTRACT"},
                                 {counter_modifier_t::operation_t::SET, "set"},
                                 {counter_modifier_t::operation_t::SET, "SET"},
                                 {counter_modifier_t::operation_t::RESET, "reset"},
                                 {counter_modifier_t::operation_t::RESET, "RESET"},
                             })
static inline void to_json(nlohmann::json& nlohmann_json_j,
                           const counter_modifier_t& nlohmann_json_t) {
    nlohmann_json_j["condition"] = nlohmann_json_t.condition;
    nlohmann_json_j["counter_key"] = nlohmann_json_t.counter_key;
    nlohmann_json_j["operation"] = nlohmann_json_t.operation;
    if (nlohmann_json_t.value) {
        nlohmann_json_j["value"] = *nlohmann_json_t.value;
    }
    if (nlohmann_json_t.counter_value) {
        nlohmann_json_j["counter_value"] = *nlohmann_json_t.counter_value;
    }
}
static inline void from_json(const nlohmann::json& nlohmann_json_j,
                             counter_modifier_t& nlohmann_json_t) {
    counter_modifier_t nlohmann_json_default_obj;
    nlohmann_json_t.condition =
        nlohmann_json_j.value("condition", nlohmann_json_default_obj.condition);
    nlohmann_json_t.counter_key =
        nlohmann_json_j.value("counter_key", nlohmann_json_default_obj.counter_key);
    nlohmann_json_t.operation =
        nlohmann_json_j.value("operation", nlohmann_json_default_obj.operation);
    if (nlohmann_json_j.contains("value")) {
        nlohmann_json_t.value = nlohmann_json_j.value("value", *nlohmann_json_default_obj.value);
    }
    if (nlohmann_json_j.contains("counter_value")) {
        nlohmann_json_t.counter_value =
            nlohmann_json_j.value("counter_value", *nlohmann_json_default_obj.counter_value);
    }
}

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_COUNTER_MODIFIER_HPP

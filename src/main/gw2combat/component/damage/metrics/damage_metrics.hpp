#ifndef GW2COMBAT_COMPONENT_DAMAGE_METRICS_DAMAGE_METRICS_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_METRICS_DAMAGE_METRICS_HPP

#include <utility>

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct damage_metrics_component {
    struct metric_unit {
        tick_t tick = 0;
        std::string source_name;
        std::string damage_name;
        double damage = 0.0;
    };

    std::vector<metric_unit> metrics;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(damage_metrics_component::metric_unit,
                                   tick,
                                   source_name,
                                   damage_name,
                                   damage)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(damage_metrics_component, metrics)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_METRICS_DAMAGE_METRICS_HPP

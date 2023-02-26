#include "audit.hpp"

#include <fstream>

#include "component/actor/combat_stats.hpp"
#include "component/actor/team.hpp"
#include "component/audit/audit_component.hpp"
#include "component/damage/incoming_damage.hpp"

#include "utils/basic_utils.hpp"
#include "utils/entity_utils.hpp"

namespace gw2combat::system {

void audit_damage(registry_t& registry) {
    registry.view<component::incoming_damage, component::audit_component>().each(
        [&](const component::incoming_damage& incoming_damage,
            component::audit_component& audit_component) {
            std::copy(incoming_damage.incoming_damage_events.begin(),
                      incoming_damage.incoming_damage_events.end(),
                      std::back_inserter(audit_component.incoming_damage_events));
        });
}

void audit_report_to_disk(registry_t& registry) {
    registry.view<component::audit_component>().each(
        [&](entity_t target_entity, const component::audit_component& audit_component) {
            if (audit_component.audit_base_path.empty()) {
                throw std::runtime_error("audit base path is empty!");
            }
            std::ofstream audit_output_stream(audit_component.audit_base_path + "/" +
                                                  utils::get_entity_name(target_entity, registry) +
                                                  "-damage-audit.csv",
                                              std::ios::trunc);
            audit_output_stream << "target|tick|source_actor|damage_source|damage" << std::endl;
            for (auto& incoming_damage_event : audit_component.incoming_damage_events) {
                std::string damage_source = [&]() {
                    if (incoming_damage_event.effect != actor::effect_t::INVALID) {
                        return utils::to_string(incoming_damage_event.effect);
                    } else if (!incoming_damage_event.skill.empty()) {
                        return incoming_damage_event.skill;
                    } else {
                        throw std::runtime_error("Unknown source for damage!");
                    }
                }();

                // spdlog::info("[{}] AUDIT: {} received {} damage due to {} by {} at tick {}",
                //              utils::get_current_tick(registry),
                //              utils::get_entity_name(target_entity, registry),
                //              incoming_damage_event.value,
                //              damage_source,
                //              utils::get_entity_name(incoming_damage_event.source_entity,
                //              registry), incoming_damage_event.tick);

                audit_output_stream
                    << utils::get_entity_name(target_entity, registry) << "|"
                    << incoming_damage_event.tick << "|"
                    << utils::get_entity_name(incoming_damage_event.source_entity, registry) << "|"
                    << damage_source << "|" << (int)incoming_damage_event.value << std::endl;
            }
        });
}

audit_report get_audit_report(registry_t& registry, bool exclude_console) {
    audit_report result;
    registry.view<component::audit_component, component::team>().each(
        [&](entity_t entity,
            const component::audit_component& audit_component,
            const component::team& team) {
            if (team.id != 2 || !result.damage_events.empty()) {
                return;
            }
            for (auto& incoming_damage_event : audit_component.incoming_damage_events) {
                if (exclude_console && utils::get_entity_name(incoming_damage_event.source_entity,
                                                              registry) == "Console") {
                    continue;
                }
                std::string damage_source = [&]() {
                    if (incoming_damage_event.effect != actor::effect_t::INVALID) {
                        return utils::to_string(incoming_damage_event.effect);
                    } else if (!incoming_damage_event.skill.empty()) {
                        return incoming_damage_event.skill;
                    } else {
                        throw std::runtime_error("Unknown source for damage!");
                    }
                }();
                result.damage_events.emplace_back(
                    audit_report::damage_event{damage_source,
                                               incoming_damage_event.tick,
                                               static_cast<int>(incoming_damage_event.value)});
            }
            auto& combat_stats = registry.get<component::combat_stats>(entity);
            result.remaining_health = combat_stats.health;
        });
    return result;
}

}  // namespace gw2combat::system

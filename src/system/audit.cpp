#include "audit.hpp"

#include <fstream>

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

void audit_report(registry_t& registry) {
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

}  // namespace gw2combat::system

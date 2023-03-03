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
            //spdlog::info("{}", utils::to_string(get_audit_report(registry)));
        });
}

audit_report_t get_audit_report(registry_t& registry) {
    audit_report_t result;
    registry.view<component::audit_component, component::team>().each(
        [&](entity_t entity,
            const component::audit_component& audit_component,
            const component::team& team) {
            if (team.id != 2 || !result.events.empty()) {
                return;
            }
            for (auto& incoming_damage_event : audit_component.incoming_damage_events) {
                auto damage_type = [&]() {
                    if (incoming_damage_event.effect != actor::effect_t::INVALID) {
                        switch (incoming_damage_event.effect) {
                            case actor::effect_t::BURNING:
                                return audit_report_t::damage_event_t::damage_type_t::BURNING;
                            case actor::effect_t::BLEEDING:
                                return audit_report_t::damage_event_t::damage_type_t::BLEEDING;
                            case actor::effect_t::TORMENT:
                                return audit_report_t::damage_event_t::damage_type_t::
                                    TORMENT_STATIONARY;
                            case actor::effect_t::POISON:
                                return audit_report_t::damage_event_t::damage_type_t::POISON;
                            case actor::effect_t::CONFUSION:
                                return audit_report_t::damage_event_t::damage_type_t::CONFUSION;
                            case actor::effect_t::BINDING_BLADE:
                                return audit_report_t::damage_event_t::damage_type_t::BINDING_BLADE;
                            default:
                                throw std::runtime_error("Unknown source for damage!");
                        }
                    } else if (!incoming_damage_event.skill.empty()) {
                        return audit_report_t::damage_event_t::damage_type_t::STRIKE;
                    } else {
                        throw std::runtime_error("Unknown source for damage!");
                    }
                }();
                auto source_skill = incoming_damage_event.skill.empty()
                                        ? "unknown_skill"
                                        : incoming_damage_event.skill;
                result.events.emplace_back(audit_report_t::damage_event_t{
                    .time_ms = incoming_damage_event.tick,
                    .actor = utils::get_entity_name(incoming_damage_event.source_entity, registry),
                    .source_skill = source_skill,
                    .damage_type = damage_type,
                    .damage = static_cast<int>(incoming_damage_event.value)});
            }
            auto& combat_stats = registry.get<component::combat_stats>(entity);
            result.remaining_health = combat_stats.health;
        });
    return result;
}

}  // namespace gw2combat::system

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
            for (auto& incoming_damage_event : incoming_damage.incoming_damage_events) {
                auto damage_type = [&]() {
                    if (incoming_damage_event.effect != actor::effect_t::INVALID) {
                        switch (incoming_damage_event.effect) {
                            case actor::effect_t::BURNING:
                                return component::audit_damage_event_t::damage_type_t::BURNING;
                            case actor::effect_t::BLEEDING:
                                return component::audit_damage_event_t::damage_type_t::BLEEDING;
                            case actor::effect_t::TORMENT:
                                return component::audit_damage_event_t::damage_type_t::
                                    TORMENT_STATIONARY;
                            case actor::effect_t::POISON:
                                return component::audit_damage_event_t::damage_type_t::POISON;
                            case actor::effect_t::CONFUSION:
                                return component::audit_damage_event_t::damage_type_t::CONFUSION;
                            case actor::effect_t::BINDING_BLADE:
                                return component::audit_damage_event_t::damage_type_t::
                                    BINDING_BLADE;
                            default:
                                throw std::runtime_error("Unknown source for damage!");
                        }
                    } else if (!incoming_damage_event.skill.empty()) {
                        return component::audit_damage_event_t::damage_type_t::STRIKE;
                    } else {
                        throw std::runtime_error("Unknown source for damage!");
                    }
                }();
                auto source_skill = incoming_damage_event.skill.empty()
                                        ? "unknown_skill"
                                        : incoming_damage_event.skill;
                audit_component.events.emplace_back(component::audit_damage_event_t{
                    .time_ms = incoming_damage_event.tick,
                    .actor = utils::get_entity_name(incoming_damage_event.source_entity, registry),
                    .source_skill = source_skill,
                    .damage_type = damage_type,
                    .damage = static_cast<int>(incoming_damage_event.value)});
            }
        });
}

void audit_report_to_disk(registry_t& registry) {
    registry.view<component::audit_component>().each(
        [&](entity_t target_entity, const component::audit_component& audit_component) {
            if (audit_component.audit_base_path.empty()) {
                spdlog::info("audit base empty for actor {}. skipping...",
                             utils::get_entity_name(target_entity, registry));
                return;
            }
            spdlog::info("writing audit for actor {}...",
                         utils::get_entity_name(target_entity, registry));
            std::ofstream audit_output_stream(audit_component.audit_base_path + "/" +
                                                  utils::get_entity_name(target_entity, registry) +
                                                  "-damage-audit.csv",
                                              std::ios::trunc);
            audit_output_stream << "target|time_ms|actor|source_skill|damage_type|damage"
                                << std::endl;
            for (auto& event : audit_component.events) {
                if (!std::holds_alternative<component::audit_damage_event_t>(event)) {
                    continue;
                }
                auto& incoming_damage_event = std::get<component::audit_damage_event_t>(event);

                audit_output_stream
                    << utils::get_entity_name(target_entity, registry) << "|"
                    << incoming_damage_event.time_ms << "|" << incoming_damage_event.actor << "|"
                    << incoming_damage_event.source_skill << "|"
                    << nlohmann::json{incoming_damage_event.damage_type}[0].get<std::string>()
                    << "|" << incoming_damage_event.damage << std::endl;
            }
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
            std::copy(audit_component.events.begin(),
                      audit_component.events.end(),
                      std::back_inserter(result.events));
            auto& combat_stats = registry.get<component::combat_stats>(entity);
            result.remaining_health = combat_stats.health;
        });
    return result;
}

}  // namespace gw2combat::system

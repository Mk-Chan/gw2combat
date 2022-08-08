#include "combat_loop.hpp"

#include "entity.hpp"
#include "skills.hpp"

#include "gw2combat/component/character/did_weapon_swap.hpp"
#include "gw2combat/component/character/downstate.hpp"
#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/character/no_more_rotation.hpp"
#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/damage/metrics/damage_metrics.hpp"
#include "gw2combat/component/damage/multipliers/incoming_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/multipliers/incoming_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/outgoing_condition_application.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/component/skills/instant_cast_skills.hpp"
#include "gw2combat/system/staged/gear_systems.hpp"
#include "gw2combat/system/staged/profession_systems.hpp"
#include "gw2combat/system/staged/skill_systems.hpp"
#include "gw2combat/system/staged/trait_systems.hpp"
#include "gw2combat/system/system.hpp"

namespace gw2combat {

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::effective_attributes,
                   component::incoming_strike_damage_multiplier,
                   component::incoming_condition_damage_multiplier,
                   component::outgoing_strike_damage_multiplier,
                   component::outgoing_condition_damage_multiplier,

                   component::outgoing_strike_damage,
                   component::incoming_strike_damage,

                   component::outgoing_condition_application,
                   component::incoming_condition_application,

                   component::effective_incoming_damage,
                   component::instant_cast_skills,
                   component::did_weapon_swap>();
}

template <combat_stage stage>
void run_staged_systems(registry_t& registry, tick_t current_tick) {
    system::virtue_of_justice<stage>(registry, current_tick);
    system::inspiring_virtue<stage>(registry, current_tick);

    system::sigil_geomancy<stage>(registry, current_tick);
    system::sigil_earth<stage>(registry, current_tick);

    system::unrelenting_criticism<stage>(registry, current_tick);
    system::symbolic_avenger<stage>(registry, current_tick);
    system::symbolic_power<stage>(registry, current_tick);

    system::on_hit_effect_applications<stage>(registry, current_tick);
}

void run_systems(registry_t& registry, tick_t current_tick) {
    system::combat_detection(registry, current_tick);

    system::effective_attributes_calculation(registry, current_tick);
    system::incoming_strike_damage_multiplier_calculation(registry, current_tick);
    system::incoming_condition_damage_multiplier_calculation(registry, current_tick);
    system::outgoing_condition_damage_multiplier_calculation(registry, current_tick);
    system::outgoing_strike_damage_multiplier_calculation(registry, current_tick);

    system::expire_effects(registry, current_tick);

    system::character_command(registry, current_tick);
    system::cast_skills(registry, current_tick);

    run_staged_systems<combat_stage::BEFORE_OUTGOING_STRIKE_BUFFERING>(registry, current_tick);

    system::incoming_strike_detection(registry, current_tick);

    run_staged_systems<combat_stage::AFTER_OUTGOING_STRIKE_BUFFERING>(registry, current_tick);
    run_staged_systems<combat_stage::BEFORE_INCOMING_STRIKE_DAMAGE_CALCULATION>(registry,
                                                                                current_tick);

    system::incoming_strike_damage_calculation(registry, current_tick);

    run_staged_systems<combat_stage::AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION>(registry,
                                                                               current_tick);

    system::incoming_condition_detection(registry, current_tick);
    system::incoming_condition_application(registry, current_tick);
    if (current_tick % effects::effect_pulse_rate == 0) {
        system::incoming_condition_damage_calculation(registry, current_tick);
    }
    system::progress_effects(registry, current_tick);

    system::update_health(registry, current_tick);
    system::downstate_detection(registry, current_tick);
    system::destroy_after_rotation_entities(registry, current_tick);

    clear_temporary_components(registry);
}

void combat_loop() {
    skills::SKILLS_DB.init("src/main/resources/skills_db.json");

    registry_t registry;
    init_entities(registry);

    tick_t current_tick{0};
    bool downstate_exit = false;
    tick_t end_tick = 1'000'000'000;
    while (true) {
        run_systems(registry, current_tick);

        bool actors_have_rotations = false;
        for (auto&& [entity] : registry.view<component::is_actor>().each()) {
            if (!registry.any_of<component::no_more_rotation>(entity)) {
                actors_have_rotations = true;
            }
        }
        for (auto&& [entity] : registry.view<component::downstate>().each()) {
            spdlog::info("tick: {}, entity: {} is downstate!",
                         current_tick,
                         utils::get_entity_name(entity, registry));
            downstate_exit = true;
        }
        if (downstate_exit) {
            break;
        }
        if (!actors_have_rotations) {
            if (end_tick == 1'000'000'000) {
                end_tick = current_tick + 10'000;
            } else if (current_tick == end_tick) {
                spdlog::info("no actor had any rotation left for 10s");
                break;
            }
        }

        current_tick += tick_t{1};
    }

    // Note: Metrics
    registry.view<component::damage_metrics_component>().each(
        [&](entity_t entity, const component::damage_metrics_component& damage_metrics_component) {
            spdlog::info("entity: {}", utils::get_entity_name(entity, registry));
            std::unordered_map<std::string, double> grouped_by_damage_name;
            for (const auto& metric_unit : damage_metrics_component.metrics) {
                grouped_by_damage_name[metric_unit.damage_name] += metric_unit.damage;
            }
            spdlog::info("{}", nlohmann::json{grouped_by_damage_name}.dump(2));

            std::unordered_map<std::string, std::unordered_map<std::string, double>>
                grouped_by_source_and_damage_name;
            for (const auto& metric_unit : damage_metrics_component.metrics) {
                grouped_by_source_and_damage_name[metric_unit.source_name]
                                                 [metric_unit.damage_name] += metric_unit.damage;
            }
            spdlog::info("{}", nlohmann::json{grouped_by_source_and_damage_name}.dump(2));
        });

    spdlog::info("tick: {}, done!", current_tick);
}

}  // namespace gw2combat

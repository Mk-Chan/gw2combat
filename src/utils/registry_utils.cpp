#include "registry_utils.hpp"

#include "entity_utils.hpp"

#include "component/actor/alacrity.hpp"
#include "component/actor/animation.hpp"
#include "component/actor/base_class_component.hpp"
#include "component/actor/begun_casting_skills.hpp"
#include "component/actor/combat_stats.hpp"
#include "component/actor/destroy_after_rotation.hpp"
#include "component/actor/finished_casting_skills.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/profession_component.hpp"
#include "component/actor/quickness.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/skills_actions_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/actor/team.hpp"
#include "component/attributes/is_attribute_conversion.hpp"
#include "component/attributes/is_attribute_modifier.hpp"
#include "component/audit/audit_component.hpp"
#include "component/counter/is_counter.hpp"
#include "component/counter/is_counter_modifier.hpp"
#include "component/damage/buffered_condition_damage.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_effect_removal.hpp"
#include "component/effect/is_skill_trigger.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/effect/source_skill.hpp"
#include "component/encounter/encounter_configuration_component.hpp"
#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"
#include "component/hierarchy/owner_component.hpp"
#include "component/lifecycle/destroy_entity.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/animation_component.hpp"
#include "component/temporal/cooldown_component.hpp"
#include "component/temporal/duration_component.hpp"
#include "component/temporal/has_alacrity.hpp"
#include "component/temporal/has_quickness.hpp"

namespace gw2combat::utils {

void copy_registry(registry_t& source_registry, registry_t& destination_registry) {
    destination_registry.ctx().emplace<tick_t>(source_registry.ctx().get<tick_t>());

    source_registry.each([&](auto entity) {
        // Create entity and copy name
        auto destination_entity = destination_registry.create(entity);
        destination_registry.ctx().emplace_as<std::string>(
            destination_entity, utils::get_entity_name(entity, source_registry));

        // Copy components
        if (source_registry.all_of<gw2combat::component::cooldown_component>(entity)) {
            destination_registry.emplace<gw2combat::component::cooldown_component>(
                destination_entity,
                source_registry.get<gw2combat::component::cooldown_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::duration_component>(entity)) {
            destination_registry.emplace<gw2combat::component::duration_component>(
                destination_entity,
                source_registry.get<gw2combat::component::duration_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::animation_component>(entity)) {
            destination_registry.emplace<gw2combat::component::animation_component>(
                destination_entity,
                source_registry.get<gw2combat::component::animation_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::owner_component>(entity)) {
            destination_registry.emplace<gw2combat::component::owner_component>(
                destination_entity,
                source_registry.get<gw2combat::component::owner_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::ammo>(entity)) {
            destination_registry.emplace<gw2combat::component::ammo>(
                destination_entity, source_registry.get<gw2combat::component::ammo>(entity));
        }
        if (source_registry.all_of<gw2combat::component::is_skill>(entity)) {
            destination_registry.emplace<gw2combat::component::is_skill>(
                destination_entity, source_registry.get<gw2combat::component::is_skill>(entity));
        }
        if (source_registry.all_of<gw2combat::component::is_effect>(entity)) {
            destination_registry.emplace<gw2combat::component::is_effect>(
                destination_entity, source_registry.get<gw2combat::component::is_effect>(entity));
        }
        if (source_registry.all_of<gw2combat::component::source_actor>(entity)) {
            destination_registry.emplace<gw2combat::component::source_actor>(
                destination_entity,
                source_registry.get<gw2combat::component::source_actor>(entity));
        }
        if (source_registry.all_of<gw2combat::component::source_skill>(entity)) {
            destination_registry.emplace<gw2combat::component::source_skill>(
                destination_entity,
                source_registry.get<gw2combat::component::source_skill>(entity));
        }
        if (source_registry.all_of<gw2combat::component::is_unique_effect>(entity)) {
            destination_registry.emplace<gw2combat::component::is_unique_effect>(
                destination_entity,
                source_registry.get<gw2combat::component::is_unique_effect>(entity));
        }
        if (source_registry.all_of<gw2combat::component::is_effect_removal>(entity)) {
            destination_registry.emplace<gw2combat::component::is_effect_removal>(
                destination_entity,
                source_registry.get<gw2combat::component::is_effect_removal>(entity));
        }
        if (source_registry.all_of<gw2combat::component::is_skill_trigger>(entity)) {
            destination_registry.emplace<gw2combat::component::is_skill_trigger>(
                destination_entity,
                source_registry.get<gw2combat::component::is_skill_trigger>(entity));
        }
        if (source_registry.all_of<gw2combat::component::is_unchained_skill_trigger>(entity)) {
            destination_registry.emplace<gw2combat::component::is_unchained_skill_trigger>(
                destination_entity,
                source_registry.get<gw2combat::component::is_unchained_skill_trigger>(entity));
        }
        if (source_registry.all_of<gw2combat::component::encounter_configuration_component>(
                entity)) {
            destination_registry.emplace<gw2combat::component::encounter_configuration_component>(
                destination_entity,
                source_registry.get<gw2combat::component::encounter_configuration_component>(
                    entity));
        }
        if (source_registry.all_of<gw2combat::component::is_attribute_conversion>(entity)) {
            destination_registry.emplace<gw2combat::component::is_attribute_conversion>(
                destination_entity,
                source_registry.get<gw2combat::component::is_attribute_conversion>(entity));
        }
        if (source_registry.all_of<gw2combat::component::is_attribute_modifier>(entity)) {
            destination_registry.emplace<gw2combat::component::is_attribute_modifier>(
                destination_entity,
                source_registry.get<gw2combat::component::is_attribute_modifier>(entity));
        }
        if (source_registry.all_of<gw2combat::component::audit_component>(entity)) {
            destination_registry.emplace<gw2combat::component::audit_component>(
                destination_entity,
                source_registry.get<gw2combat::component::audit_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::is_counter>(entity)) {
            destination_registry.emplace<gw2combat::component::is_counter>(
                destination_entity, source_registry.get<gw2combat::component::is_counter>(entity));
        }
        if (source_registry.all_of<gw2combat::component::is_counter_modifier_t>(entity)) {
            destination_registry.emplace<gw2combat::component::is_counter_modifier_t>(
                destination_entity,
                source_registry.get<gw2combat::component::is_counter_modifier_t>(entity));
        }
        if (source_registry.all_of<gw2combat::component::team>(entity)) {
            destination_registry.emplace<gw2combat::component::team>(
                destination_entity, source_registry.get<gw2combat::component::team>(entity));
        }
        if (source_registry.all_of<gw2combat::component::begun_casting_skills>(entity)) {
            destination_registry.emplace<gw2combat::component::begun_casting_skills>(
                destination_entity,
                source_registry.get<gw2combat::component::begun_casting_skills>(entity));
        }
        if (source_registry.all_of<gw2combat::component::static_attributes>(entity)) {
            destination_registry.emplace<gw2combat::component::static_attributes>(
                destination_entity,
                source_registry.get<gw2combat::component::static_attributes>(entity));
        }
        if (source_registry.all_of<gw2combat::component::animation>(entity)) {
            destination_registry.emplace<gw2combat::component::animation>(
                destination_entity, source_registry.get<gw2combat::component::animation>(entity));
        }
        if (source_registry.all_of<gw2combat::component::combat_stats>(entity)) {
            destination_registry.emplace<gw2combat::component::combat_stats>(
                destination_entity,
                source_registry.get<gw2combat::component::combat_stats>(entity));
        }
        if (source_registry.all_of<gw2combat::component::skills_actions_component>(entity)) {
            destination_registry.emplace<gw2combat::component::skills_actions_component>(
                destination_entity,
                source_registry.get<gw2combat::component::skills_actions_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::finished_skills_actions_component>(
                entity)) {
            destination_registry.emplace<gw2combat::component::finished_skills_actions_component>(
                destination_entity,
                source_registry.get<gw2combat::component::finished_skills_actions_component>(
                    entity));
        }
        if (source_registry.all_of<gw2combat::component::finished_casting_skills>(entity)) {
            destination_registry.emplace<gw2combat::component::finished_casting_skills>(
                destination_entity,
                source_registry.get<gw2combat::component::finished_casting_skills>(entity));
        }
        if (source_registry.all_of<gw2combat::component::base_class_component>(entity)) {
            destination_registry.emplace<gw2combat::component::base_class_component>(
                destination_entity,
                source_registry.get<gw2combat::component::base_class_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::rotation_component>(entity)) {
            destination_registry.emplace<gw2combat::component::rotation_component>(
                destination_entity,
                source_registry.get<gw2combat::component::rotation_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::profession_component>(entity)) {
            destination_registry.emplace<gw2combat::component::profession_component>(
                destination_entity,
                source_registry.get<gw2combat::component::profession_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::weapon_t>(entity)) {
            destination_registry.emplace<gw2combat::component::weapon_t>(
                destination_entity, source_registry.get<gw2combat::component::weapon_t>(entity));
        }
        if (source_registry.all_of<gw2combat::component::equipped_weapons>(entity)) {
            destination_registry.emplace<gw2combat::component::equipped_weapons>(
                destination_entity,
                source_registry.get<gw2combat::component::equipped_weapons>(entity));
        }
        if (source_registry.all_of<gw2combat::component::current_weapon_set>(entity)) {
            destination_registry.emplace<gw2combat::component::current_weapon_set>(
                destination_entity,
                source_registry.get<gw2combat::component::current_weapon_set>(entity));
        }
        if (source_registry.all_of<gw2combat::component::bundle_component>(entity)) {
            destination_registry.emplace<gw2combat::component::bundle_component>(
                destination_entity,
                source_registry.get<gw2combat::component::bundle_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::equipped_bundle>(entity)) {
            destination_registry.emplace<gw2combat::component::equipped_bundle>(
                destination_entity,
                source_registry.get<gw2combat::component::equipped_bundle>(entity));
        }
        if (source_registry.all_of<gw2combat::component::dropped_bundle>(entity)) {
            destination_registry.emplace<gw2combat::component::dropped_bundle>(
                destination_entity,
                source_registry.get<gw2combat::component::dropped_bundle>(entity));
        }
        if (source_registry.all_of<gw2combat::component::strike_t>(entity)) {
            destination_registry.emplace<gw2combat::component::strike_t>(
                destination_entity, source_registry.get<gw2combat::component::strike_t>(entity));
        }
        if (source_registry.all_of<gw2combat::component::incoming_strike>(entity)) {
            destination_registry.emplace<gw2combat::component::incoming_strike>(
                destination_entity,
                source_registry.get<gw2combat::component::incoming_strike>(entity));
        }
        if (source_registry.all_of<gw2combat::component::outgoing_strikes_component>(entity)) {
            destination_registry.emplace<gw2combat::component::outgoing_strikes_component>(
                destination_entity,
                source_registry.get<gw2combat::component::outgoing_strikes_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::incoming_strikes_component>(entity)) {
            destination_registry.emplace<gw2combat::component::incoming_strikes_component>(
                destination_entity,
                source_registry.get<gw2combat::component::incoming_strikes_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::condition_damage_t>(entity)) {
            destination_registry.emplace<gw2combat::component::condition_damage_t>(
                destination_entity,
                source_registry.get<gw2combat::component::condition_damage_t>(entity));
        }
        if (source_registry.all_of<gw2combat::component::buffered_condition_damage>(entity)) {
            destination_registry.emplace<gw2combat::component::buffered_condition_damage>(
                destination_entity,
                source_registry.get<gw2combat::component::buffered_condition_damage>(entity));
        }
        if (source_registry.all_of<gw2combat::component::effect_application_t>(entity)) {
            destination_registry.emplace<gw2combat::component::effect_application_t>(
                destination_entity,
                source_registry.get<gw2combat::component::effect_application_t>(entity));
        }
        if (source_registry.all_of<gw2combat::component::outgoing_effects_component>(entity)) {
            destination_registry.emplace<gw2combat::component::outgoing_effects_component>(
                destination_entity,
                source_registry.get<gw2combat::component::outgoing_effects_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::incoming_effect_application>(entity)) {
            destination_registry.emplace<gw2combat::component::incoming_effect_application>(
                destination_entity,
                source_registry.get<gw2combat::component::incoming_effect_application>(entity));
        }
        if (source_registry.all_of<gw2combat::component::incoming_effects_component>(entity)) {
            destination_registry.emplace<gw2combat::component::incoming_effects_component>(
                destination_entity,
                source_registry.get<gw2combat::component::incoming_effects_component>(entity));
        }
        if (source_registry.all_of<gw2combat::component::incoming_damage_event>(entity)) {
            destination_registry.emplace<gw2combat::component::incoming_damage_event>(
                destination_entity,
                source_registry.get<gw2combat::component::incoming_damage_event>(entity));
        }
        if (source_registry.all_of<gw2combat::component::incoming_damage>(entity)) {
            destination_registry.emplace<gw2combat::component::incoming_damage>(
                destination_entity,
                source_registry.get<gw2combat::component::incoming_damage>(entity));
        }

        // Copy tags
        if (source_registry.all_of<gw2combat::component::has_quickness>(entity)) {
            destination_registry.emplace<gw2combat::component::has_quickness>(destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::cooldown_expired>(entity)) {
            destination_registry.emplace<gw2combat::component::cooldown_expired>(
                destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::duration_expired>(entity)) {
            destination_registry.emplace<gw2combat::component::duration_expired>(
                destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::animation_expired>(entity)) {
            destination_registry.emplace<gw2combat::component::animation_expired>(
                destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::has_alacrity>(entity)) {
            destination_registry.emplace<gw2combat::component::has_alacrity>(destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::destroy_entity>(entity)) {
            destination_registry.emplace<gw2combat::component::destroy_entity>(destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::is_damaging_effect>(entity)) {
            destination_registry.emplace<gw2combat::component::is_damaging_effect>(
                destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::quickness>(entity)) {
            destination_registry.emplace<gw2combat::component::quickness>(destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::destroy_after_rotation>(entity)) {
            destination_registry.emplace<gw2combat::component::destroy_after_rotation>(
                destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::alacrity>(entity)) {
            destination_registry.emplace<gw2combat::component::alacrity>(destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::is_downstate>(entity)) {
            destination_registry.emplace<gw2combat::component::is_downstate>(destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::combat_stats_updated>(entity)) {
            destination_registry.emplace<gw2combat::component::combat_stats_updated>(
                destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::relative_attributes>(entity)) {
            destination_registry.emplace<gw2combat::component::relative_attributes>(
                destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::is_actor>(entity)) {
            destination_registry.emplace<gw2combat::component::is_actor>(destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::actor_created>(entity)) {
            destination_registry.emplace<gw2combat::component::actor_created>(destination_entity);
        }
        if (source_registry.all_of<gw2combat::component::no_more_rotation>(entity)) {
            destination_registry.emplace<gw2combat::component::no_more_rotation>(
                destination_entity);
        }
    });
}

}  // namespace gw2combat::utils

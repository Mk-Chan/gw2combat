#include "system.hpp"

#include "gw2combat/component/actor/animation.hpp"
#include "gw2combat/component/actor/finished_casting_skill.hpp"
#include "gw2combat/component/actor/quickness.hpp"

namespace gw2combat::system {

void do_animation(registry_t& registry) {
    registry.view<component::animation>().each(
        [&](entity_t entity, component::animation& animation) {
            if (animation.duration[0] == 0) {
                registry.emplace<component::finished_casting_skill>(
                    entity, component::finished_casting_skill{animation.skill});
                registry.remove<component::animation>(entity);
                return;
            }

            int no_quickness_progress_pct = animation.progress[0] * 100 / animation.duration[0];
            int quickness_progress_pct = animation.progress[1] * 100 / animation.duration[1];

            if (no_quickness_progress_pct + quickness_progress_pct >= 100) {
                registry.emplace<component::finished_casting_skill>(
                    entity, component::finished_casting_skill{animation.skill});
                registry.remove<component::animation>(entity);
                return;
            }

            bool has_quickness = registry.any_of<component::quickness>(entity);
            animation.progress[0] += !has_quickness;
            animation.progress[1] += has_quickness;
        });
}

}  // namespace gw2combat::system

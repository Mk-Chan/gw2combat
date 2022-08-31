#include "system.hpp"

#include "gw2combat/component/effect/duration.hpp"

namespace gw2combat::system {

void progress_durations(registry_t& registry) {
    registry.view<component::duration>().each([&](entity_t entity, component::duration& duration) {
        ++duration.progress;
        if (duration.progress >= duration.duration) {
            registry.emplace<component::duration_expired>(entity);
        }
    });
}

}  // namespace gw2combat::system

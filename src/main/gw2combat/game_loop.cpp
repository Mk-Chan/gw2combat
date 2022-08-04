#include "game_loop.hpp"

#include <spdlog/spdlog.h>

#include "entity.hpp"
#include "skills.hpp"
#include "system/system.hpp"

#include "gw2combat/component/character/downstate.hpp"
#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/gear/weapon_configurations.hpp"
#include "gw2combat/component/skills/normal_cast_skill.hpp"

namespace gw2combat {

void game_loop() {
    skills::SKILLS_DB.init("src/main/resources/skills_db.json");
    // spdlog::info("{}", nlohmann::json{skills::SKILLS_DB}.dump());
    // return;

    registry_t registry;

    init_entities(registry);

    tick_t current_tick{0};
    bool reported_downstate = false;
    while (true) {
        system::run_systems(registry, current_tick);

        for (auto&& [entity] : registry.view<component::downstate>().each()) {
            spdlog::info("tick: {}, entity: {} is downstate!",
                         current_tick,
                         utils::get_entity_name(entity, registry));
            reported_downstate = true;
        }
        if (reported_downstate) {
            break;
        }

        current_tick += tick_t{1};
    }
    spdlog::info("tick: {}, done!", current_tick);
}

}  // namespace gw2combat

#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/character_input.hpp"

namespace gw2combat::system {

void character_command(context& ctx) {
    ctx.registry.view<component::character_input>().each(
        [&](const entt::entity entity, component::character_input& character_input) {
            character_input.input_bitset = component::character_input::command::NOTHING;
            character_input.received_at = ctx.current_tick;

            // TODO: Read the input command here. Following is testing code
            //       Currently just spamming 1 every tick
            if (entity == entt::entity{1}) {
                character_input.input_bitset = component::character_input::make_command(
                    {component::character_input::command::WEAPON_SKILL_1});
                character_input.received_at = ctx.current_tick;
            }

            //spdlog::info("entity: {}, received at tick: {}, character_input_bitset: {}",
            //             static_cast<std::uint32_t>(entity),
            //             character_input.received_at,
            //             static_cast<std::uint32_t>(character_input.input_bitset));
        });
}

}  // namespace gw2combat::system

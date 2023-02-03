#include "common.hpp"

#include "system/encounter.hpp"

namespace gw2combat {

void combat_loop() {
    registry_t registry;

    system::encounter(registry);
}

}  // namespace gw2combat

int main() {
    gw2combat::combat_loop();
    return 0;
}

#include "combat_loop.hpp"

int main(int argc, char** argv) {
    std::string encounter_configuration_path = [&]() {
        if (argc == 1) {
            return std::string{"resources/encounter.json"};
        } else if (argc == 2) {
            return std::string{argv[1]};
        } else {
            throw std::runtime_error("Usage: ./gw2combat <optional:encounter.json path>");
        }
    }();
    gw2combat::combat_loop(encounter_configuration_path);
    return 0;
}

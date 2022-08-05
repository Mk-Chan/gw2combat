#ifndef GW2COMBAT_COMPONENT_EFFECTS_EFFECT_COMPONENTS_HPP
#define GW2COMBAT_COMPONENT_EFFECTS_EFFECT_COMPONENTS_HPP

#include "gw2combat/effects.hpp"

namespace gw2combat::component {

// Boons
struct aegis {
    using effect_type = effects::aegis;
    effect_type effect;
};
struct alacrity {
    using effect_type = effects::alacrity;
    effect_type effect;
};
struct fury {
    using effect_type = effects::fury;
    effect_type effect;
};
struct might {
    using effect_type = effects::might;
    effects::stacking_effect<effect_type> effect{25};
};
struct quickness {
    using effect_type = effects::quickness;
    effect_type effect;
};
struct resolution {
    using effect_type = effects::resolution;
    effect_type effect;
};

// Non-damaging conditions
struct vulnerability {
    using effect_type = effects::vulnerability;
    effects::stacking_effect<effect_type> effect{25};
};

// Damaging conditions
struct burning {
    using effect_type = effects::burning;
    effects::stacking_effect<effect_type> effect{1500};
};
struct bleeding {
    using effect_type = effects::bleeding;
    effects::stacking_effect<effect_type> effect{1500};
};

// Unique damaging conditions
struct binding_blade {
    using effect_type = effects::binding_blade;
    effect_type effect;
};
struct symbolic_avenger_effect {
    using effect_type = effects::symbolic_avenger;
    effects::stacking_effect<effect_type> effect{5};
};
struct virtue_of_justice_effect {
    using effect_type = effects::virtue_of_justice;
    effect_type effect;
};
struct inspiring_virtue_effect {
    using effect_type = effects::inspiring_virtue;
    effect_type effect;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECTS_EFFECT_COMPONENTS_HPP

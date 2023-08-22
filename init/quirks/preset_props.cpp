#include "../quirks.h"

#include <android-base/properties.h>

// Disable navbar by default on devices known to have physical keys
PRESET_PROPS(
    FP_STARTS_WITH("Unihertz/Jelly") || FP_STARTS_WITH("Unihertz/Atom"),
    "persist.sys.gsi.hw.mainkeys", "1"
);

// Some xiaomi devices: Override default network mode to enable 5G
PRESET_PROPS(
    FP_STARTS_WITH("Redmi/gauguin"),
    "ro.telephony.default_network", "33,22"
);

#include "../quirks.h"

#include <android-base/properties.h>

// Disable navbar by default on devices known to have physical keys
PRESET_PROPS(
    (FP_STARTS_WITH("Unihertz/Jelly") && !FP_STARTS_WITH("Unihertz/Jelly_Max")) || FP_STARTS_WITH("Unihertz/Atom"),
    "persist.sys.gsi.hw.mainkeys", "1"
);

// Unihertz Jelly Max: Broken "fake" gravity sensor. Ignore it to use AOSP's SensorFusion
PRESET_PROPS(
    FP_STARTS_WITH("Unihertz/Jelly_Max"),
    "persist.sys.phh.sensor_filter", "mtk:gravity"
);

// Some xiaomi devices: Override default network mode to enable 5G
PRESET_PROPS(
    FP_STARTS_WITH("Redmi/gauguin"),
    "ro.telephony.default_network", "33,22"
);

// Qualcomm: No stereo VoIP
PRESET_PROPS(
    PROP_STARTS_WITH("ro.boot.hardware", "qcom"),
    "persist.sys.phh.disable_stereo_voip", "true"
);

// MediaTek GED
PRESET_PROPS(
    PROP_STARTS_WITH("ro.hardware.hwcomposer", "mtk"),
    "persist.sys.phh.mtk_ged_kpi", "1"
);

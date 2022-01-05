#include "../../quirks.h"

#include <android-base/properties.h>

#include <filesystem>

using namespace std;

class UlefonePowerArmor13Quirks : DeviceQuirk {
public:
    bool ShouldRun() {
        return FP_STARTS_WITH("Ulefone/Power_Armor_13/");
    }
    
    void Run() {
        // Set the A2DP offload properties to disabled so legacy policy is loaded
        android::base::SetProperty("persist.bluetooth.a2dp_offload.disabled", "true");
        android::base::SetProperty("persist.bluetooth.bluetooth_audio_hal.disabled", "true");
    }
};

LOAD_QUIRK(UlefonePowerArmor13Quirks);

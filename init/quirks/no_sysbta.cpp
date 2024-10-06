#include "../quirks.h"
#include <android-base/properties.h>
#include <filesystem>

using namespace std;

// sysbta is unsupported for AIDL audio HAL -- hopefully they won't need it either
// If they do, we'll figure something out later...
class NoSysbtaForAidl : DeviceQuirk {
public:
    bool ShouldRun() {
        // AIDL audio HAL has no audio_policy_configuration.xml
        return !filesystem::exists("/vendor/etc/audio_policy_configuration.xml");
    }

    void Run() {
        android::base::SetProperty("persist.bluetooth.system_audio_hal.enabled", "false");
    }
};

LOAD_QUIRK(NoSysbtaForAidl);

#include "../../quirks.h"

#include <android-base/properties.h>

#include <filesystem>

using namespace std;

class CafAudioQuirk : DeviceQuirk {
public:
    bool ShouldRun() {
        return filesystem::exists("/vendor/etc/audio/audio_policy_configuration.xml");
    }
    
    void Run() {
        Quirks::OverrideWithBindMount("/vendor/etc/audio/audio_policy_configuration.xml", "/vendor/etc/audio_policy_configuration.xml");
        if (filesystem::exists("/vendor/etc/a2dp_audio_policy_configuration.xml")) {
            Quirks::OverrideFileReplaceSubstr("/vendor/etc/a2dp_audio_policy_configuration.xml", "bluetooth_qti", "a2dp");
        }

        // Disable A2DP offload on qcom
        android::base::SetProperty("persist.bluetooth.a2dp_offload.disabled", "true");
        android::base::SetProperty("persist.bluetooth.bluetooth_audio_hal.disabled", "true");
    }
};

LOAD_QUIRK(CafAudioQuirk);

#include "../../quirks.h"

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
    }
};

LOAD_QUIRK(CafAudioQuirk);

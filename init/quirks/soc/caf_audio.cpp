#include "../../quirks.h"
#include <android-base/properties.h>
#include <filesystem>

using namespace std;

class CafAudioQuirk : DeviceQuirk {
public:
    bool ShouldRun() {
        return filesystem::exists("/vendor/etc/audio/audio_policy_configuration.xml");
    }
    
    void RunPostData() {
        if (android::base::GetBoolProperty("persist.sys.gsi.alternate_audio_policy", false)) {
            Quirks::OverrideWithBindMount("/vendor/etc/audio/audio_policy_configuration.xml", "/vendor/etc/audio_policy_configuration.xml");
            android::base::SetProperty("ctl.restart", "audioserver"); // Ensure our new policy is loaded into audioserver
        }
    }
};

LOAD_QUIRK(CafAudioQuirk);

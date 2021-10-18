#include "../../quirks.h"

#include <android-base/properties.h>

#include <filesystem>

using namespace std;

class UnihertzKeylayoutQuirk : DeviceQuirk {
public:
    bool ShouldRun() {
        return FP_STARTS_WITH("Unihertz/");
    }
    
    void Run() {
        Quirks::OverrideFolderWith("/system/usr/keylayout", [](auto p) {
            Quirks::CopyFileKeepPerms("/system/system_ext/quirks/keylayout/unihertz-fingerprint_key.kl", p / "fingerprint_key.kl");
            Quirks::CopyFileKeepPerms("/system/system_ext/quirks/keylayout/unihertz-mtk-tpd.kl", p / "mtk-tpd.kl");
            Quirks::CopyFileKeepPerms("/system/system_ext/quirks/keylayout/unihertz-mtk-tpd-kpd.kl", p / "mtk-tpd-kpd.kl");
            Quirks::CopyFileKeepPerms("/system/system_ext/quirks/keylayout/unihertz-mtk-kpd.kl", p / "mtk-kpd.kl");
        });
    }
};

LOAD_QUIRK(UnihertzKeylayoutQuirk);

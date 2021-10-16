#include "../../quirks.h"

#include <filesystem>

using namespace std;

const string PATHS[] = {
    "/vendor/lib/libmtk-ril.so",
    "/vendor/lib/mtk-ril.so",
    "/vendor/lib64/libmtk-ril.so",
    "/vendor/lib64/mtk-ril.so"
};

class MtkRilQuirk : DeviceQuirk {
public:
    MtkRilQuirk() {
        Quirks::Add(this);
    }
    
    bool ShouldRun() {
        for (auto& p : PATHS) {
            if (filesystem::exists(p)) {
                return true;
            }
        }
        return false;
    }
    
    void Run() {
        for (auto& p : PATHS) {
            if (filesystem::exists(p)) {
                Quirks::OverrideFileReplaceSubstr(p, "AT\\+EAIC=2", "AT+EAIC=3");
            }
        }
    }
};

static MtkRilQuirk* _ignored = new MtkRilQuirk();

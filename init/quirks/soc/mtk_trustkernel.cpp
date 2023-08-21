#include "../../quirks.h"

#include <android-base/properties.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include <utility>

using namespace std;

#define AVB_PROP_OS_VERSION "com.android.build.boot.os_version"
#define AVB_PROP_SPL "com.android.build.boot.security_patch"

optional<pair<string, string>> try_get_spl() {
    string boot_part = "/dev/block/by-name/boot" + android::base::GetProperty("ro.boot.slot_suffix", "");

    // Read from AVB footer, https://github.com/TrebleDroid/device_phh_treble/blob/android-13.0/fixSPL/getSPL.c
    // First read into memory
    int fd = open(boot_part.c_str(), O_RDONLY);
    off_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char *buf = (char *) malloc(size);
    read(fd, buf, size);

    // Search for AVB footer property directly in the binary
    char* p = (char *) memmem(buf, size, AVB_PROP_OS_VERSION, sizeof(AVB_PROP_OS_VERSION));
    if (p == nullptr)
        return nullopt;

    p += sizeof(AVB_PROP_OS_VERSION);
    string os_version(p);

    p = (char *) memmem(buf, size, AVB_PROP_SPL, sizeof(AVB_PROP_SPL));
    if (p == nullptr)
        return nullopt;

    p += sizeof(AVB_PROP_SPL);
    string spl(p);

    return make_pair(os_version, spl);
}

class MtkTkQuirk : DeviceQuirk {
public:
    bool ShouldRun() {
        return filesystem::exists("/proc/tkcore/tkcore_log")
                 // No longer an issue after MediaTek upgraded to KeyMint AIDL services
                 && !filesystem::exists("/vendor/bin/hw/android.hardware.security.keymint-service.trustkernel");
    }

    void Run() {
        android::base::SetProperty("debug.phh.props.ice.trustkernel", "keymaster");
        android::base::SetProperty("debug.phh.props.teed", "keymaster");
        android::base::SetProperty("ro.keymaster.brn", "Android");
        android::base::SetProperty("ro.keymaster.mod", "AOSP on ARM64");

        auto res = try_get_spl();

        if (res) {
            android::base::SetProperty("ro.keymaster.xxx.release", res->first);
            android::base::SetProperty("ro.keymaster.xxx.security_patch", res->second);
        }

        android::base::SetProperty("ctl.restart", "teed");
    }
};

LOAD_QUIRK(MtkTkQuirk)

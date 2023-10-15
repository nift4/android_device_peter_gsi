#include "../../quirks.h"

#include <android-base/properties.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <utility>

using namespace std;

#define AVB_PROP_OS_VERSION "com.android.build.boot.os_version"
#define AVB_PROP_SPL "com.android.build.boot.security_patch"

map<int, string> api_to_version{
    {30, "11"}, {31, "12"}, {32, "12L"},
    {33, "13"}, {34, "14"}
};

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
        return filesystem::exists("/proc/tkcore/tkcore_log");
    }

    void Run() {
        android::base::SetProperty("debug.phh.props.ice.trustkernel", "keymaster");
        android::base::SetProperty("debug.phh.props.teed", "keymaster");
        android::base::SetProperty("ro.keymaster.brn", "Android");
        android::base::SetProperty("ro.keymaster.mod", "AOSP on ARM64");

	string release = android::base::GetProperty("ro.vendor.build.version.release", "11");
	string spl = android::base::GetProperty("ro.vendor.build.version.security_patch", "2023-01-05");
        auto res = try_get_spl();

        if (res) {
            release = res->first;
            spl = res->second;
        }

        // With GRF, release version from vendor or boot may not be what we need
        int first_api_level = android::base::GetIntProperty("ro.product.first_api_level", 30);
        if (api_to_version.count(first_api_level) > 0) {
            string release_from_first_api = api_to_version[first_api_level];
            if (stoi(release_from_first_api) >= stoi(release)) {
                release = release_from_first_api;
            }
        }

        android::base::SetProperty("ro.keymaster.xxx.release", release);
        android::base::SetProperty("ro.keymaster.xxx.security_patch", spl);

        android::base::SetProperty("ctl.restart", "teed");
    }
};

LOAD_QUIRK(MtkTkQuirk)

#define LOG_TAG "init_gsi_quirks"
#include <utils/Log.h>

#include "quirks.h"

#include <vector>

#include <sys/mount.h>

// Default destructor for DeviceQuirk
DeviceQuirk::~DeviceQuirk() {
    
}

static std::vector<DeviceQuirk*>* quirks = nullptr;

void Quirks::Add(DeviceQuirk* quirk) {
    if (quirks == nullptr) {
        quirks = new std::vector<DeviceQuirk*>();
    }
    quirks->push_back(quirk);
}

void Quirks::Run() {
    if (quirks == nullptr) return;
    
    for (DeviceQuirk* quirk : *quirks) {
        if (quirk->ShouldRun()) {
            quirk->Run();
        }
    }
}

// Utility functions for use with quirks
#define QUIRKS_TMP_BASE_PATH "/mnt/quirks"
#define QUIRKS_TMP_FILES_PATH QUIRKS_TMP_BASE_PATH "/files"

void EnsureDirectory(filesystem::path p) {
    if (!filesystem::is_directory(p)) {
        filesystem::create_directories(p);
    }
}

void EnsureTmpMount() {
    if (filesystem::is_directory(QUIRKS_TMP_BASE_PATH)) return;
    
    EnsureDirectory(QUIRKS_TMP_BASE_PATH);
    int err = mount("tmpfs", QUIRKS_TMP_BASE_PATH, "tmpfs", 0, "mode=755,gid=0");
    
    if (err < 0) {
        ALOGE("mount tmpfs on %s err = %d\n", QUIRKS_TMP_BASE_PATH, errno);
    }
}

void Quirks::OverrideFileReplaceSubstr(filesystem::path p, string pattern, string replacement) {
    if (!filesystem::is_regular_file(p)) return;
    
    EnsureTmpMount();
    EnsureDirectory(QUIRKS_TMP_FILES_PATH);
    
    filesystem::path tmp_path = QUIRKS_TMP_FILES_PATH + p.string();
    EnsureDirectory(tmp_path.parent_path());
    
    // TODO: Actually implement pattern replacement
    filesystem::copy_file(p, tmp_path);
    
    int err = mount(tmp_path.c_str(), p.c_str(), nullptr, MS_BIND, nullptr);
    
    if (err < 0) {
        ALOGE("bind mount %s on %s err = %d\n", tmp_path.c_str(), p.c_str(), errno);
    }
    
    // Call restorecon via execl, because for some reason
    // libselinux functions will segfault in our case
    // (probably related to other magic present in our process)
    fork_execl("/system/bin/restorecon", "restorecon", p.c_str());
}

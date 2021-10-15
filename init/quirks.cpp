#define LOG_TAG "init_gsi_quirks"
#include <utils/Log.h>

#include "quirks.h"

#include <vector>

#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

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

void Quirks::OverrideFileWith(filesystem::path p, function<void(istream&, ostream&)> proc) {
    if (!filesystem::is_regular_file(p)) return;
    
    EnsureTmpMount();
    EnsureDirectory(QUIRKS_TMP_FILES_PATH);
    
    filesystem::path tmp_path = QUIRKS_TMP_FILES_PATH + p.string();
    EnsureDirectory(tmp_path.parent_path());
    
    ifstream ifs;
    ifs.open(p, ifstream::in);
    
    ofstream ofs(tmp_path);
    proc(ifs, ofs);
    
    ifs.close();
    ofs.close();
    
    // Synchronize ownership and permission
    // C++ filesystem does not support uid / gid manipulation
    struct stat st;
    int err = stat(p.c_str(), &st);
    if (err < 0) {
        ALOGE("Failed to stat %s: %d\n", tmp_path.c_str(), errno);
        return;
    }
    
    err = chown(tmp_path.c_str(), st.st_uid, st.st_gid);
    if (err < 0) {
        ALOGE("Failed to chown %s: %d\n", tmp_path.c_str(), errno);
    }
    err = chmod(tmp_path.c_str(), st.st_mode);
    if (err < 0) {
        ALOGE("Failed to chmod %s: %d\n", tmp_path.c_str(), errno);
    }
    
    // Bind mount and override the file
    err = mount(tmp_path.c_str(), p.c_str(), nullptr, MS_BIND, nullptr);
    
    if (err < 0) {
        ALOGE("bind mount %s on %s err = %d\n", tmp_path.c_str(), p.c_str(), errno);
        return;
    }
    
    // Call restorecon via execl, because for some reason
    // libselinux functions will segfault in our case
    // (probably related to other magic present in our process)
    fork_execl("/system/bin/restorecon", "restorecon", p.c_str());
}

void Quirks::OverrideFileReplaceSubstr(filesystem::path p, string pattern, string replacement) {
    Quirks::OverrideFileWith(p, [](istream& is, ostream& os) {
        os << is.rdbuf();
    });
}

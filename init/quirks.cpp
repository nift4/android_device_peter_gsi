#define LOG_TAG "init_gsi_quirks"
#include <utils/Log.h>

#include "quirks.h"

#include <regex>
#include <sstream>
#include <vector>

#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

// Default constructor to add self to the loaded list of quirks
DeviceQuirk::DeviceQuirk() {
    Quirks::Add(this);
}

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

void Quirks::RunPostData() {
    if (quirks == nullptr) return;
    
    for (DeviceQuirk* quirk : *quirks) {
        if (quirk->ShouldRun()) {
            quirk->RunPostData();
        }
    }
}

// Utility functions for use with quirks
#define QUIRKS_TMP_BASE_PATH "/mnt/quirks"
#define QUIRKS_TMP_FILES_PATH QUIRKS_TMP_BASE_PATH "/files"
#define QUIRKS_TMP_DIRS_PATH QUIRKS_TMP_BASE_PATH "/dirs"

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

void RestoreFilePermissions(filesystem::path orig, filesystem::path new_path) {
    // Synchronize ownership and permission
    // C++ filesystem does not support uid / gid manipulation
    struct stat st;
    int err = stat(orig.c_str(), &st);
    if (err < 0) {
        ALOGE("Failed to stat %s: %d\n", orig.c_str(), errno);
        return;
    }
    
    err = chown(new_path.c_str(), st.st_uid, st.st_gid);
    if (err < 0) {
        ALOGE("Failed to chown %s: %d\n", new_path.c_str(), errno);
    }
    err = chmod(new_path.c_str(), st.st_mode);
    if (err < 0) {
        ALOGE("Failed to chmod %s: %d\n", new_path.c_str(), errno);
    }
}

void Quirks::CopyFileKeepPerms(filesystem::path src, filesystem::path dst) {
    filesystem::copy_file(src, dst);
    RestoreFilePermissions(src, dst);
}

int Quirks::OverrideWithBindMount(filesystem::path src, filesystem::path dst) {
    int err = mount(src.c_str(), dst.c_str(), nullptr, MS_BIND, nullptr);

    if (err < 0) {
        ALOGE("bind mount %s on %s err = %d\n", src.c_str(), dst.c_str(), errno);
    }

    return err;
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
    
    RestoreFilePermissions(p, tmp_path);
    
    // Bind mount and override the file
    int err = OverrideWithBindMount(tmp_path, p);

    if (err < 0) {
        return;
    }
    
    // Call restorecon via execl, because for some reason
    // libselinux functions will segfault in our case
    // (probably related to other magic present in our process)
    fork_execl("/system/bin/restorecon", "restorecon", p.c_str());
}

void Quirks::OverrideFileReplaceSubstr(filesystem::path p, string pattern, string replacement) {
    Quirks::OverrideFileWith(p, [pattern, replacement](istream& is, ostream& os) {
        string str = string((istreambuf_iterator<char>(is)), istreambuf_iterator<char>());
        os << regex_replace(str, regex(pattern), replacement);;
    });
}

void Quirks::OverrideFolderWith(filesystem::path p, function<void(filesystem::path)> proc) {
    if (!filesystem::is_directory(p)) return;
    
    EnsureTmpMount();
    EnsureDirectory(QUIRKS_TMP_DIRS_PATH);
    
    filesystem::path tmp_path = QUIRKS_TMP_DIRS_PATH + p.string();
    EnsureDirectory(tmp_path);
    
    filesystem::copy(p, tmp_path, filesystem::copy_options::recursive);
    
    for (auto& entry : filesystem::recursive_directory_iterator(tmp_path)) {
        if (!filesystem::is_regular_file(entry.path())) continue;
        RestoreFilePermissions(p / filesystem::relative(entry.path(), tmp_path), entry.path());
    }
    
    // Restore the permission of the outer directory as well
    RestoreFilePermissions(p, tmp_path);
    
    proc(tmp_path);
    
    int err = OverrideWithBindMount(tmp_path, p);
    
    if (err < 0) {
        return;
    }
    
    fork_execl("/system/bin/restorecon", "restorecon", "-R", p.c_str());
}

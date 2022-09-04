#pragma once
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include <sys/wait.h>
#include <unistd.h>

#define PROP_STARTS_WITH(prop, prefix) \
    (android::base::GetProperty(prop, "").rfind(prefix, 0) == 0)
#define FP_STARTS_WITH(prefix) \
    PROP_STARTS_WITH("ro.vendor.build.fingerprint", prefix)

using namespace std;

template<typename... Args>
void fork_execl(Args... args) {
    int pid, status;
    if ((pid = fork()) == 0) {
        execl(args..., nullptr);
    } else {
        waitpid(pid, &status, 0);
    }
}

class DeviceQuirk {
public:
    DeviceQuirk();
    virtual bool ShouldRun() = 0;
    virtual void Run() {}
    virtual void RunPostData() {}
    virtual ~DeviceQuirk();
};

#define LOAD_QUIRK(NAME) \
    static NAME _ignored;

namespace Quirks {
    void Add(DeviceQuirk* quirk);
    void Run();
    void RunPostData();

    int OverrideWithBindMount(filesystem::path src, filesystem::path dst);

    void OverrideFileWith(filesystem::path p, function<void(istream&, ostream&)> proc);
    void OverrideFileReplaceSubstr(filesystem::path p, string pattern, string replacement);
    
    void OverrideFolderWith(filesystem::path p, function<void(filesystem::path)> proc);
    
    void CopyFileKeepPerms(filesystem::path src, filesystem::path dst);
}

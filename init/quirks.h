#pragma once
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include <sys/wait.h>
#include <unistd.h>

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
    virtual void Run() = 0;
    virtual ~DeviceQuirk();
};

namespace Quirks {
    void Add(DeviceQuirk* quirk);
    void Run();
    
    void OverrideFileWith(filesystem::path p, function<void(istream&, ostream&)> proc);
    void OverrideFileReplaceSubstr(filesystem::path p, string pattern, string replacement);
    
    void OverrideFolderWith(filesystem::path p, function<void(filesystem::path)> proc);
    
    void CopyFileKeepPerms(filesystem::path src, filesystem::path dst);
}

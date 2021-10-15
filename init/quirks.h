#pragma once
#include <filesystem>
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
    virtual bool ShouldRun();
    virtual void Run();
    virtual ~DeviceQuirk();
};

namespace Quirks {
    void Add(DeviceQuirk* quirk);
    void Run();
    
    void OverrideFileReplaceSubstr(filesystem::path p, string pattern, string replacement);
}

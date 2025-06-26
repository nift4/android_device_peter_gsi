// vim: expandtab tabstop=4 shiftwidth=4

#include <android/binder_process.h>
#include <android/binder_manager.h>
#include <aidl/vendor/mediatek/hardware/agolddaemon/IAgoldDaemon.h>

int main(int argc, char **argv) {
    if (argc != 2) return 2;
    int val = !!atoi(argv[1]);
    ABinderProcess_startThreadPool();
    auto svc = aidl::vendor::mediatek::hardware::agolddaemon::IAgoldDaemon::fromBinder(
        ndk::SpAIBinder(AServiceManager_waitForService("vendor.mediatek.hardware.agolddaemon.IAgoldDaemon/default")));
    if (svc != nullptr) {
        int ret = 0;
        svc->SendMessageToIoctl(100, 0, val, val, &ret);
        return ret;
    }
    return 1;
}

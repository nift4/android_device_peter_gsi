// vim: expandtab tabstop=4 shiftwidth=4

#include <android/binder_process.h>
#include <android/binder_manager.h>
#include <aidl/vendor/mediatek/hardware/agolddaemon/IAgoldDaemon.h>

int main(int, char **) {
    ABinderProcess_startThreadPool();
    auto svc = aidl::vendor::mediatek::hardware::agolddaemon::IAgoldDaemon::fromBinder(
        ndk::SpAIBinder(AServiceManager_waitForService("vendor.mediatek.hardware.agolddaemon.IAgoldDaemon/default")));
    if (svc != nullptr) {
        svc->setNotGsi("NotGsi");
        return 0;
    }
    return 1;
}

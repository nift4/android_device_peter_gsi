#include <android-base/logging.h>
#include <android-base/properties.h>

#include <string>
#include <iostream>

using namespace android;
using namespace std;

// From libresetprop
extern int setprop(const char *name, const char *value, bool trigger);

void override_ro_prop(string prefix, string source, string postfix, string value) {
    if (value.length() == 0) return;
    
    std::string prop = prefix;
    
    if (source.length() > 0) {
        prop += ".";
        prop += source;
    }
    
    prop += "." + postfix;
    std::cout << prop << std::endl;
    std::cout << value << std::endl;
    
    setprop(prop.c_str(), value.c_str(), true);
}

std::string RO_PROP_SOURCES[] = {
    "", "product", "odm", "vendor", "system_ext", "system", "bootimage",
};

int main() {
    string device = base::GetProperty("ro.product.vendor.device", "");
    string model = base::GetProperty("ro.product.vendor.model", "");
    string name = base::GetProperty("ro.product.vendor.name", "");
    string brand = base::GetProperty("ro.product.vendor.brand", "");
    string manufacturer = base::GetProperty("ro.product.vendor.manufacturer", "");
    string fingerprint = base::GetProperty("ro.vendor.build.fingerprint", "");
    
    for (const auto& source : RO_PROP_SOURCES) {
        override_ro_prop("ro.product", source, "device", device);
        override_ro_prop("ro.product", source, "model", model);
        override_ro_prop("ro.product", source, "name", name);
        override_ro_prop("ro.product", source, "brand", brand);
        override_ro_prop("ro.product", source, "manufacturer", manufacturer);
        override_ro_prop("ro", source, "build.fingerprint", fingerprint);
    }
    
    return 0;
}

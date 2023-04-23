// vim: expandtab tabstop=4 shiftwidth=4
#include <iostream>
#include <string>

#include <vendor/mediatek/hardware/aguiledbelt/1.0/IAguiLedBeltLight.h>
#include <vendor/mediatek/hardware/aguiledbelt/1.0/types.h>

using namespace ::vendor::mediatek::hardware::aguiledbelt::V1_0;

void print_usage() {
    std::cerr << "Usage: aguiledbeltctl <light_bit_field> <r> <g> <b> <brightness>" << std::endl;
    std::cerr << "       aguiledbeltctl clear" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<light_bit_field> should be a binary string where each bit (0 or 1) corresponds to the ON state of each physical light belt" << std::endl;
}

int main(int argc, char **argv) {
    auto service = IAguiLedBeltLight::getService();

    if (argc <= 1) {
        print_usage();
        return -1;
    }

    std::string arg1(argv[1]);

    if (arg1 == "clear") {
        service->setLedBeltAlwaysOnState(0, 0, 0, false);
        return 0;
    }

    uint32_t light_id = std::stoi(std::string(argv[1]), nullptr, 2);

    if (light_id > 31) {
        std::cerr << "First argument should be a bit field with each bit corresponding to one physical light belt" << std::endl;
        return 0;
    }

    if (argc < 6) {
        print_usage();
        return -1;
    }

    uint32_t r = std::stoi(std::string(argv[2]));
    uint32_t g = std::stoi(std::string(argv[3]));
    uint32_t b = std::stoi(std::string(argv[4]));

    if (r > 255 || g > 255 || b > 255) {
        std::cerr << "Color channels must be bewteen 0 and 255" << std::endl;
        return -1;
    }

    uint32_t brightness = std::stoi(std::string(argv[5]));

    if (brightness > 100) {
        std::cerr << "Brightness must be between 0 and 100" << std::endl;
        return -1;
    }

    service->setLedBeltAlwaysOnState(light_id, (r << 16) + (g << 8) + b, brightness, false);
    return 0;
}

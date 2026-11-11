#include "../app/src/ble/gadgetbridge/ble_gadgetbridge.c"
#include "selftest_common.h"

int main(int argc, char **argv)
{
    uint8_t buf[4096];
    if (argc < 2) {
        return 2;
    }
    size_t n = hex_decode(argv[1], buf, sizeof(buf));
    shim_reset();
    ble_gadgetbridge_input(buf, (uint16_t)n);
    dump_event();
    return 0;
}

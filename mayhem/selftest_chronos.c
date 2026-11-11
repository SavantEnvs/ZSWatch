#include "../app/src/ble/chronos/ble_chronos.c"
#include "selftest_common.h"

int main(int argc, char **argv)
{
    uint8_t buf[4096];
    if (argc < 2) {
        return 2;
    }
    setenv("TZ", "UTC", 1);
    tzset();
    size_t n = hex_decode(argv[1], buf, sizeof(buf));
    shim_reset();
    ble_chronos_on_receive_data(buf, (uint16_t)n);
    dump_event();
    return 0;
}

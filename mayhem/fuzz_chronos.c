// Fuzzes the Chronos BLE protocol decoder (multi-fragment 0xAB/0xEA packets: notifications, weather, navigation, config).
// The upstream translation unit is included so its reassembly state can be reset per input.
#include "../app/src/ble/chronos/ble_chronos.c"
#include "shim/shim_state.h"

// Return every parser static to its start-up state so an input never sees what an earlier one left.
static void reset_parser_state(void)
{
    for (int i = 0; i < CH_NOTIF_SIZE; i++) {
        free(notifications[i].title);
        free(notifications[i].message);
    }
    memset(notifications, 0, sizeof(notifications));
    notificationIndex = -1;

    free(caller_name);
    caller_name = NULL;
    free(app_info.version);
    memset(&app_info, 0, sizeof(app_info));
    free(weather_info.city);
    memset(&weather_info, 0, sizeof(weather_info));
    char **nav_fields[] = {&navigation.title, &navigation.duration, &navigation.distance,
                           &navigation.eta, &navigation.directions};
    for (size_t i = 0; i < sizeof(nav_fields) / sizeof(nav_fields[0]); i++) {
        free(*nav_fields[i]);
    }
    memset(&navigation, 0, sizeof(navigation));

    memset(weather, 0, sizeof(weather));
    memset(hourly_forecast, 0, sizeof(hourly_forecast));
    memset(&remote_touch, 0, sizeof(remote_touch));
    memset(&phone_info, 0, sizeof(phone_info));

    memset(&incoming, 0, sizeof(incoming));
    incoming_received = 0;
    incoming_sequence = 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 2 || size > 4096) {
        return -1;
    }
    reset_parser_state();
    shim_event_owns_strings = 1;
    shim_reset();

    size_t chunk = data[0];
    data++;
    size--;
    if (chunk == 0) {
        chunk = size;
    }
    for (size_t off = 0; off < size; off += chunk) {
        size_t n = size - off < chunk ? size - off : chunk;
        ble_chronos_on_receive_data(data + off, (uint16_t)n);
    }
    return 0;
}

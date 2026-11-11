// Known-answer probes: feed one message (hex on argv[1]) to the parser and print what it emitted.
#include <stdio.h>
#include <stdlib.h>
#include "shim/shim_state.h"

static size_t hex_decode(const char *hex, uint8_t *out, size_t cap)
{
    size_t n = 0;
    for (; hex[0] && hex[1] && n < cap; hex += 2) {
        char b[3] = { hex[0], hex[1], 0 };
        out[n++] = (uint8_t)strtoul(b, NULL, 16);
    }
    return n;
}

static void dump_event(void)
{
    const ble_comm_cb_data_t *e = &shim_last_event.data;
    printf("events=%u sent=%u\n", shim_event_count, shim_sent_count);
    if (shim_event_count == 0) {
        return;
    }
    switch (e->type) {
        case BLE_COMM_DATA_TYPE_NOTIFY:
            printf("type=notify id=%u src=%s title=%s body=%s\n", (unsigned)e->data.notify.id,
                   e->data.notify.src ? e->data.notify.src : "(null)",
                   e->data.notify.title ? e->data.notify.title : "(null)",
                   e->data.notify.body ? e->data.notify.body : "(null)");
            break;
        case BLE_COMM_DATA_TYPE_NOTIFY_REMOVE:
            printf("type=notify_remove id=%u\n", (unsigned)e->data.notify_remove.id);
            break;
        case BLE_COMM_DATA_TYPE_SET_TIME:
            printf("type=set_time seconds=%u\n", (unsigned)e->data.time.seconds);
            break;
        case BLE_COMM_DATA_TYPE_WEATHER:
            printf("type=weather temp=%d hum=%u wind=%u code=%u text=%s\n", e->data.weather.temperature_c,
                   e->data.weather.humidity, e->data.weather.wind, e->data.weather.weather_code,
                   e->data.weather.report_text);
            break;
        case BLE_COMM_DATA_TYPE_MUSIC_INFO:
            printf("type=music_info artist=%s album=%s track=%s duration=%d\n", e->data.music_info.artist,
                   e->data.music_info.album, e->data.music_info.track_name, e->data.music_info.duration);
            break;
        case BLE_COMM_DATA_TYPE_MUSIC_STATE:
            printf("type=music_state playing=%d position=%d shuffle=%d repeat=%d\n", e->data.music_state.playing,
                   e->data.music_state.position, e->data.music_state.shuffle, e->data.music_state.repeat);
            break;
        case BLE_COMM_DATA_TYPE_GPS:
            printf("type=gps lat=%.3f lon=%.3f sats=%u\n", e->data.gps.lat, e->data.gps.lon, e->data.gps.satellites);
            break;
        default:
            printf("type=%d\n", (int)e->type);
            break;
    }
}

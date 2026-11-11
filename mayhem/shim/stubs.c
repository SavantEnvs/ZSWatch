/* Host stubs for the firmware services the BLE parsers talk to. */
#include <stdlib.h>
#include <string.h>
#include "shim_state.h"
#include "ble/ble_comm.h"
#include "events/ble_event.h"
#include "ble/gadgetbridge/ble_find_device.h"
#include "applications/watchface/watchface_app.h"
#include <zephyr/zbus/zbus.h>

static struct ble_data_event event_storage;
struct zbus_channel ble_comm_data_chan = { &event_storage, sizeof(event_storage) };

struct ble_data_event shim_last_event;
unsigned shim_event_count;
uint8_t shim_last_sent[4096];
size_t shim_last_sent_len;
unsigned shim_sent_count;

int shim_event_owns_strings;

static void release_event_strings(void)
{
    if (shim_event_owns_strings && shim_event_count > 0 && shim_last_event.data.type == BLE_COMM_DATA_TYPE_NOTIFY) {
        ble_comm_notify_t *n = &shim_last_event.data.data.notify;
        free(n->src);
        free(n->sender);
        free(n->title);
        free(n->subject);
        free(n->body);
    }
}

void shim_reset(void)
{
    release_event_strings();
    memset(&shim_last_event, 0, sizeof(shim_last_event));
    shim_event_count = 0;
    shim_last_sent_len = 0;
    shim_sent_count = 0;
}

int shim_zbus_pub(const struct zbus_channel *chan, const void *msg)
{
    if (chan == &ble_comm_data_chan) {
        release_event_strings();
        memcpy(&shim_last_event, msg, sizeof(shim_last_event));
        shim_event_count++;
    }
    return 0;
}

int ble_comm_send(const uint8_t *data, uint16_t len)
{
    shim_last_sent_len = len < sizeof(shim_last_sent) ? len : sizeof(shim_last_sent);
    memcpy(shim_last_sent, data, shim_last_sent_len);
    shim_sent_count++;
    return len;
}
void ble_comm_short_interval_request(enum ble_comm_short_interval_reason r) { (void)r; }
void ble_comm_short_interval_release(enum ble_comm_short_interval_reason r) { (void)r; }
int ble_comm_get_mtu(void) { return 247; }
int ble_find_device_set(bool enabled) { (void)enabled; return 0; }
void ble_find_device_disconnected(void) {}
int watchface_app_reload_bg(void) { return 0; }
int watchface_app_reset_bg(void) { return 0; }

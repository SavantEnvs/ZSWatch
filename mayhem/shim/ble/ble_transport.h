#pragma once
#include <stdint.h>
struct bt_conn;
int ble_transport_send(struct bt_conn *connection, const uint8_t *data, uint16_t length);

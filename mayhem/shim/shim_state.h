#pragma once
#include <stddef.h>
#include <stdint.h>
#include "events/ble_event.h"

extern struct ble_data_event shim_last_event;
extern unsigned shim_event_count;
extern uint8_t shim_last_sent[4096];
extern size_t shim_last_sent_len;
extern unsigned shim_sent_count;
void shim_reset(void);
/* Set by a harness whose parser hands heap strings to the event consumer: the stub frees them. */
extern int shim_event_owns_strings;

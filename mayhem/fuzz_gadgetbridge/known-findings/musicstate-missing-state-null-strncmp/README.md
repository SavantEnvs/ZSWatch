# parse_musicstate: strncmp on NULL when "state" is missing

`app/src/ble/gadgetbridge/ble_gadgetbridge.c` `parse_musicstate()`: `extract_value_str("\"state\":", ...)` returns
NULL when the phone's message has no `"state"` key, and the result goes straight into `strncmp(temp_value, "play", temp_len)`.
UBSan reports `null pointer passed as argument 1, which is declared to never be null`; on the watch it is a NULL read.

Reproducer: `./fuzz_gadgetbridge-standalone mayhem/fuzz_gadgetbridge/known-findings/musicstate-missing-state-null-strncmp/reproducer`
(first byte is the harness's BLE fragment-size selector, the rest is the message).

Fix: check `temp_value != NULL` before the `strncmp`.

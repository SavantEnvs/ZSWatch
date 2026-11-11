// Fuzzes the Gadgetbridge BLE message parser (GB({...}) framing, setTime, JSON/JS-literal commands).
// The upstream translation unit is included so its parser state can be reset per input.
#include "../app/src/ble/gadgetbridge/ble_gadgetbridge.c"
#include "shim/shim_state.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 2 || size > 4096) {
        return -1;
    }
    num_parsed_brackets = 0;
    parse_state = WAIT_GB;
    parsed_data_index = 0;
    memset(receive_buf, 0, sizeof(receive_buf));
    shim_reset();

    // First byte picks the BLE fragment size (0 = one write) so the multi-packet state machine is exercised.
    size_t chunk = data[0];
    data++;
    size--;
    if (chunk == 0) {
        chunk = size;
    }
    for (size_t off = 0; off < size; off += chunk) {
        size_t n = size - off < chunk ? size - off : chunk;
        ble_gadgetbridge_input(data + off, (uint16_t)n);
    }
    return 0;
}

# ble_chronos_data_received: `incoming.data[8] << 24` shifts into the sign bit

`ble_chronos.c` (0xEF navigation data, `data[5] == 0x80`): `(uint32_t)(incoming.data[8] << 24)` promotes the byte to `int`
first, so any value >= 0x80 shifts a 1 into the sign bit, which is undefined behaviour (UBSan `shift-base`). Harmless with
the ARM compiler in practice, but it fires on every such packet, so the harness build relaxes that one check
(`-fno-sanitize=shift-base`).

Reproducer: build with the check enabled (drop `$RELAX` in mayhem/build.sh) and run it on this input.
Fix: cast before shifting: `((uint32_t)incoming.data[8] << 24)`.

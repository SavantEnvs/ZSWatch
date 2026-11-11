# parse_gps_data: -1 stored into uint64 through a double (float-cast-overflow)

`cb.data.data.gps.time = time != NULL ? time->valuedouble : -1;` converts a negative double to `uint64_t`, which is
undefined behaviour in C. Benign on Cortex-M in practice, but UBSan's `float-cast-overflow` fires on every GPS message
without a `time` field, so the harness build relaxes that one check (`-fno-sanitize=float-cast-overflow`).

Reproducer: build with the check enabled (drop `$RELAX` in mayhem/build.sh) and run it on this input.
Fix: use `(uint64_t)-1` or an explicit validity flag.

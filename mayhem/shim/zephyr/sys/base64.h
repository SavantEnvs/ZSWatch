#pragma once
#include <zephyr/kernel.h>

/* Same contract as Zephyr's base64_decode (mbedtls-derived): padding handled, -EINVAL on bad input, -ENOMEM if dst is too small. */
static inline int base64_decode(uint8_t *dst, size_t dlen, size_t *olen, const uint8_t *src, size_t slen)
{
    static const char tbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t o = 0;
    uint32_t acc = 0;
    int bits = 0;
    size_t pad = 0;
    for (size_t i = 0; i < slen; i++) {
        uint8_t c = src[i];
        if (c == '=') {
            pad++;
            continue;
        }
        if (pad) {
            return -EINVAL;
        }
        const char *p = c ? strchr(tbl, c) : NULL;
        if (!p) {
            return -EINVAL;
        }
        acc = (acc << 6) | (uint32_t)(p - tbl);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            if (o >= dlen) {
                *olen = o;
                return -ENOMEM;
            }
            dst[o++] = (uint8_t)(acc >> bits);
        }
    }
    *olen = o;
    return 0;
}

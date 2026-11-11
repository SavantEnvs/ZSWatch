/* Host stand-in for the Zephyr kernel API used by the BLE parsers. */
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define ARG_UNUSED(x) (void)(x)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

typedef struct { int64_t ticks; } k_timeout_t;
#define K_MSEC(ms) ((k_timeout_t){ (ms) })
#define K_NO_WAIT ((k_timeout_t){ 0 })
#define K_FOREVER ((k_timeout_t){ -1 })

#define k_malloc(n) malloc(n)
#define k_calloc(n, s) calloc((n), (s))
#define k_free(p) free(p)
static inline int32_t k_sleep(k_timeout_t t)
{
    (void)t;
    return 0;
}

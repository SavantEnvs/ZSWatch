#pragma once
#include <stdio.h>
#define LOG_MODULE_REGISTER(...) _Static_assert(1, "log")
#define LOG_MODULE_DECLARE(...) _Static_assert(1, "log")
#define SHIM_LOG(...) do { if (0) { (void)snprintf(NULL, 0, __VA_ARGS__); } } while (0)
#define LOG_ERR(...) SHIM_LOG(__VA_ARGS__)
#define LOG_WRN(...) SHIM_LOG(__VA_ARGS__)
#define LOG_INF(...) SHIM_LOG(__VA_ARGS__)
#define LOG_DBG(...) SHIM_LOG(__VA_ARGS__)
#define LOG_HEXDUMP_DBG(d, l, s) do { (void)(d); (void)(l); (void)(s); } while (0)
#define LOG_HEXDUMP_INF(d, l, s) LOG_HEXDUMP_DBG(d, l, s)

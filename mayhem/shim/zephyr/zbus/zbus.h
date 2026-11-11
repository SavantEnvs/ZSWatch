#pragma once
#include <zephyr/kernel.h>

struct zbus_channel {
    void *message;
    size_t message_size;
};

/* Implemented by the harness: records the last published message. */
int shim_zbus_pub(const struct zbus_channel *chan, const void *msg);

#define ZBUS_CHAN_DECLARE(...) extern struct zbus_channel __VA_ARGS__
#define ZBUS_LISTENER_DEFINE(name, cb) \
    static void (*const name##_shim_cb)(const struct zbus_channel *) __attribute__((unused)) = (cb)

static inline int zbus_chan_pub(const struct zbus_channel *chan, const void *msg, k_timeout_t t)
{
    (void)t;
    return shim_zbus_pub(chan, msg);
}
static inline const void *zbus_chan_const_msg(const struct zbus_channel *chan)
{
    return chan->message;
}

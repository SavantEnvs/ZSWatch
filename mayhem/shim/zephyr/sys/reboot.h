#pragma once
#define SYS_REBOOT_WARM 0
#define SYS_REBOOT_COLD 1
static inline void sys_reboot(int type)
{
    (void)type;
}

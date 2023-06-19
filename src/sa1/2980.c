#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"

#define VI_BUFTEST_ADDR_REG (VI_BASE_REG + 0x38)
#define VI_BUFTEST_DATA_REG (VI_BASE_REG + 0x3C)

#define VI_CTRL_TEST_ENABLE (1 <<  7)
#define VI_CTRL_KILL_WE     (1 << 11)

void func_80003980(void) {
    u32 saveCtrl = IO_READ(VI_CONTROL_REG);

    IO_WRITE(VI_CONTROL_REG, VI_CTRL_TEST_ENABLE | VI_CTRL_KILL_WE);

    IO_WRITE(VI_BUFTEST_ADDR_REG, 0);
    IO_WRITE(VI_BUFTEST_DATA_REG, 0x43210123);

    IO_WRITE(VI_BUFTEST_ADDR_REG, 1);
    IO_WRITE(VI_BUFTEST_DATA_REG, 0);

    IO_WRITE(VI_BUFTEST_ADDR_REG, 2);
    IO_WRITE(VI_BUFTEST_DATA_REG, 0);

    IO_WRITE(VI_BUFTEST_ADDR_REG, 3);
    IO_WRITE(VI_BUFTEST_DATA_REG, 0);

    IO_WRITE(VI_CONTROL_REG, saveCtrl);
}

s32 func_800039DC(void) {
    u32 saveCtrl = IO_READ(VI_CONTROL_REG);
    u32 data;

    IO_WRITE(VI_CONTROL_REG, VI_CTRL_TEST_ENABLE | VI_CTRL_KILL_WE);

    IO_WRITE(VI_BUFTEST_ADDR_REG, 0);
    data = IO_READ(VI_BUFTEST_DATA_REG);

    IO_WRITE(VI_CONTROL_REG, saveCtrl);

    if (data == 0x43210123) {
        return TRUE;
    }
    return FALSE;
}

s32 func_80003A28(void* arg0) {
    s32 i = 0;
    u32 saveMask = __osDisableInt();

    bzero(arg0, 0x1900);

    do {
        u32 temp;

        IO_WRITE(VI_CONTROL_REG, 0);
        __osBbDelay(10);
        temp = IO_READ(MI_30_REG) & ~0x02000000;
        IO_WRITE(MI_30_REG, temp);
        __osBbDelay(1);
        IO_WRITE(MI_30_REG, temp | 0x02000000);

        IO_WRITE(VI_ORIGIN_REG, arg0);

        // NTSC LAN1 configuration
        IO_WRITE(VI_WIDTH_REG, 320);
        IO_WRITE(VI_INTR_REG, 520);
        IO_WRITE(VI_BURST_REG, (62 << 20) | (5 << 16) | (34 << 8) | 57);
        IO_WRITE(VI_V_SYNC_REG, 525);
        IO_WRITE(VI_H_SYNC_REG, 3093);
        IO_WRITE(VI_LEAP_REG, (3093 << 16) | 3093);
        IO_WRITE(VI_H_START_REG, (108 << 16) | 748);
        IO_WRITE(VI_V_START_REG, (37 << 16) | 511);
        IO_WRITE(VI_V_BURST_REG, (0 << 20) | (14 << 16) | (2 << 8) | 4);
        IO_WRITE(VI_X_SCALE_REG, 0x200);
        IO_WRITE(VI_Y_SCALE_REG, 0x400);

        func_80003980();

        IO_WRITE(VI_CURRENT_REG, 0);

        IO_WRITE(VI_CONTROL_REG, VI_CTRL_PIXEL_ADV_1 | VI_CTRL_ANTIALIAS_MODE_2 | VI_CTRL_GAMMA_ON |
                 VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_TYPE_16);

        while (IO_READ(VI_CURRENT_REG) < 48) {
            ;
        }

        i++;
    } while (func_800039DC());

    IO_WRITE(VI_H_START_REG, 0);

    __osRestoreInt(saveMask);

    return i - 1;
}

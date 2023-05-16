#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/misc/rtc", write_rtc);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/misc/rtc", send_start);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/misc/rtc", send_stop);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/misc/rtc", read_bytes);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/misc/rtc", write_bytes);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/misc/rtc", osBbRtcInit);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/misc/rtc", osBbRtcSet);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/misc/rtc", osBbRtcGet);

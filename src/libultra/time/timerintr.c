#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/time/timerintr", __osTimerServicesInit);

INCLUDE_ASM("asm/nonmatchings/libultra/time/timerintr", __osTimerInterrupt);

INCLUDE_ASM("asm/nonmatchings/libultra/time/timerintr", __osSetTimerIntr);

INCLUDE_ASM("asm/nonmatchings/libultra/time/timerintr", __osInsertTimer);

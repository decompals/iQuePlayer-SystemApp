#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/pi/piacs", __osPiCreateAccessQueue);

INCLUDE_ASM("asm/nonmatchings/libultra/pi/piacs", __osPiGetAccess);

INCLUDE_ASM("asm/nonmatchings/libultra/pi/piacs", __osPiRelAccess);

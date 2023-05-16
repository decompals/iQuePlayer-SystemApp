#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libgcc/_udivdi3", __udivdi3);

INCLUDE_RODATA("asm/nonmatchings/libgcc/_udivdi3", __clz_tab);

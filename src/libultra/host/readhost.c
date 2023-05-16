#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/host/readhost", __osReadHost);

INCLUDE_ASM("asm/nonmatchings/libultra/host/readhost", osReadHost);

INCLUDE_ASM("asm/nonmatchings/libultra/host/readhost", osBbReadHost);

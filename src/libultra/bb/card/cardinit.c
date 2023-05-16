#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardinit", __osBbCardPresent);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardinit", __osBbCardRelAccess);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardinit", __osBbCardGetAccess);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardinit", osBbCardInit);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardinit", __osBbCardDmaCopy);

INCLUDE_RODATA("asm/nonmatchings/libultra/bb/card/cardinit", __osBbCardConfig);

#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/carderase", erase_block);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/carderase", erase_blocks);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/carderase", osBbCardEraseBlock);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/carderase", osBbCardEraseBlocks);

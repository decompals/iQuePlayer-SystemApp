#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardwrite", fill_page);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardwrite", write_page);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardwrite", write_dummy);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardwrite", osBbCardWriteBlock);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardwrite", osBbCardWriteBlocks);

#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardstatus", osBbCardUnhappy);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardstatus", __osBbCardStatus);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardstatus", osBbCardStatus);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardstatus", osBbCardChange);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardstatus", osBbCardClearChange);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/card/cardstatus", osBbCardBlocks);

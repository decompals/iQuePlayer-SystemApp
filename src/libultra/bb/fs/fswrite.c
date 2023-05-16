#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fswrite", __osBbFReallocBlock);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fswrite", osBbFWrite);

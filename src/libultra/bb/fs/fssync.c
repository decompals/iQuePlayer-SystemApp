#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fssync", __osBbFsSync);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fssync", osBbFAutoSync);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fssync", osBbFSync);

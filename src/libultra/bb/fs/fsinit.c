#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fsinit", createAccessQueue);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fsinit", __osBbFsGetAccess);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fsinit", __osBbFsRelAccess);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fsinit", csum);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fsinit", __osBbFCheck);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fsinit", __osBbFsReadFat);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fsinit", __osBbFsReadFatLinks);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fsinit", osBbFInit);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/fs/fsinit", osBbFStatFs);

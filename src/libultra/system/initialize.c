#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/system/initialize", __createSpeedParam);

INCLUDE_ASM("asm/nonmatchings/libultra/system/initialize", __osInitialize_common);

INCLUDE_ASM("asm/nonmatchings/libultra/system/initialize", __osInitialize_autodetect);

#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/system/initialize_emu", proutSyncPrintf);

INCLUDE_ASM("asm/nonmatchings/libultra/system/initialize_emu", __osInitialize_emu);

#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/system/initialize_kmc", rmonPutchar);

INCLUDE_ASM("asm/nonmatchings/libultra/system/initialize_kmc", kmc_proutSyncPrintf);

INCLUDE_ASM("asm/nonmatchings/libultra/system/initialize_kmc", __osInitialize_kmc);

INCLUDE_ASM("asm/nonmatchings/libultra/system/initialize_kmc", __checkHardware_kmc);

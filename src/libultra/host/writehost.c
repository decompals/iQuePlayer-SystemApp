#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/host/writehost", osBbInitWriteHost);

INCLUDE_ASM("asm/nonmatchings/libultra/host/writehost", __osWriteHost);

INCLUDE_ASM("asm/nonmatchings/libultra/host/writehost", osWriteHost);

INCLUDE_ASM("asm/nonmatchings/libultra/host/writehost", osBbWriteHost);

INCLUDE_ASM("asm/nonmatchings/libultra/host/writehost", osBbSyncHost);

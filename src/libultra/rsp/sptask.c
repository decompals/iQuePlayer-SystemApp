#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/rsp/sptask", _VirtualToPhysicalTask);

INCLUDE_ASM("asm/nonmatchings/libultra/rsp/sptask", osSpTaskLoad);

INCLUDE_ASM("asm/nonmatchings/libultra/rsp/sptask", osSpTaskStartGo);

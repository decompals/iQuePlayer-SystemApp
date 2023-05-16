#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skGetId);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", symbol);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skLaunchSetup);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skLaunch);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skRecryptListValid);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skRecryptBegin);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skRecryptData);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skRecryptComputeState);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skRecryptEnd);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skSignHash);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skVerifyHash);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skGetConsumption);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skAdvanceTicketWindow);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skSetLimit);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skExit);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skKeepAlive);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skGetRandomKeyData);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skDumpVirage);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skTest2);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skTest3);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/sk/skapi", skResetWindow);

#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libz/inflate", inflateReset);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", inflateInit2_);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", inflateInit_);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", fixedtables);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", updatewindow);

INCLUDE_RODATA("asm/nonmatchings/libz/inflate", lenfix);

INCLUDE_RODATA("asm/nonmatchings/libz/inflate", distfix);

INCLUDE_RODATA("asm/nonmatchings/libz/inflate", order);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", inflate);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", inflateEnd);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", inflateSetDictionary);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", syncsearch);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", inflateSync);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", inflateSyncPoint);

INCLUDE_ASM("asm/nonmatchings/libz/inflate", inflateCopy);

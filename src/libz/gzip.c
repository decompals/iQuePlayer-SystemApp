#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libz/gzip", myalloc);

INCLUDE_ASM("asm/nonmatchings/libz/gzip", myfree);

INCLUDE_ASM("asm/nonmatchings/libz/gzip", expand_gzip);

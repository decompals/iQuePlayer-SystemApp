#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libz/inftrees", inflate_table);

INCLUDE_RODATA("asm/nonmatchings/libz/inftrees", lbase);

INCLUDE_RODATA("asm/nonmatchings/libz/inftrees", lext);

INCLUDE_RODATA("asm/nonmatchings/libz/inftrees", dbase);

INCLUDE_RODATA("asm/nonmatchings/libz/inftrees", dext);

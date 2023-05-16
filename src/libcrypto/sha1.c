#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libcrypto/sha1", SHA1Transform);

INCLUDE_ASM("asm/nonmatchings/libcrypto/sha1", SHA1Reset);

INCLUDE_ASM("asm/nonmatchings/libcrypto/sha1", SHA1Input);

INCLUDE_ASM("asm/nonmatchings/libcrypto/sha1", SHA1Result);

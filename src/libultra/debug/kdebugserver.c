#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/debug/kdebugserver", string_to_u32);

INCLUDE_ASM("asm/nonmatchings/libultra/debug/kdebugserver", send_packet);

INCLUDE_ASM("asm/nonmatchings/libultra/debug/kdebugserver", clear_IP6);

INCLUDE_ASM("asm/nonmatchings/libultra/debug/kdebugserver", send);

INCLUDE_ASM("asm/nonmatchings/libultra/debug/kdebugserver", kdebugserver);

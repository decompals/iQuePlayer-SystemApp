#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11un", _usb_dci_vusb11_unstall_endpoint);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11un", _usb_dci_vusb11_cancel_transfer);

#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11st", _usb_dci_vusb11_get_endpoint_status);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11st", _usb_dci_vusb11_set_endpoint_status);

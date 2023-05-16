#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_sts", _usb_device_unstall_endpoint);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_sts", _usb_device_get_status);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_sts", _usb_device_set_status);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_sts", _usb_device_stall_endpoint);

#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_mand", _usb_device_init);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_mand", _usb_device_register_service);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_mand", _usb_device_init_endpoint);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_mand", _usb_device_get_transfer_status);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_mand", _usb_device_read_setup_data);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_mand", _usb_device_recv_data);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/dev_mand", _usb_device_call_service);

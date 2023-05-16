#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbinit", __osBbUsbInterrupt);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbinit", __osBbUsbRead);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbinit", __osBbUsbWrite);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbinit", __osBbUsbMgrProc);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbinit", __osBbUsbThreadInit);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbinit", osBbUsbSetCtlrModes);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbinit", osBbUsbInit);

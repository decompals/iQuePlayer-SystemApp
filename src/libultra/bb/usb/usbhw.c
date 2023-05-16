#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbhw", __usb_splhigh);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbhw", __usb_splx);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbhw", __usbDevInterrupt);

void __usbDevRead(void) {
}

void __usbDevWrite(void) {
}

void __usbCtlrTest(void) {
}

void __usbHostMode(void) {
}

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbhw", __usbDeviceMode);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbhw", __usbOtgStateChange);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbhw", osBbUsbGetResetCount);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbhw", __usbCtlrInit);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbhw", __usbHwInit);

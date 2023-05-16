#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9GetStatus);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9ClearFeature);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9SetFeature);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9SetAddress);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9GetDescription);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9SetDescription);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9GetConfig);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9SetConfig);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9GetInterface);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9SetInterface);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9SynchFrame);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", ch9Vendor);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", dev_service_ep0);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", dev_reset_ep0);

void dev_bus_suspend(void) {
}

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", dev_bus_sof);

void _usb_device_state_machine(void) {
}

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/usbdevice", __usb_arc_device_setup);

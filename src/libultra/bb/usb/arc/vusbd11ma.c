#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11ma", _usb_dci_vusb11_init);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11ma", _usb_dci_vusb11_isr);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11ma", _usb_dci_vusb11_bus_reset);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11ma", _usb_dci_vusb11_submit_BDT);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11ma", _usb_dci_vusb11_read_setup);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11ma", _usb_dci_vusb11_submit_transfer);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/arc/vusbd11ma", _usb_dci_vusb11_init_endpoint);

#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"


INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", __osRdb_Usb_GetAvailSendBytes);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", osRdb_Usb_StartSend);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", rdbs_service_bulk_ep);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", rdb_ip6_input);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", rdbs_ch9GetDescription);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", rdbs_init_data_eps);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", rdbs_reset_data_eps);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", rdbs_stall_data_eps);

void rdbs_ch9Vendor(void) {
}

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", rdbs_reset_ep0);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", rdbs_query);

INCLUDE_ASM("asm/nonmatchings/libultra/bb/usb/rdb_slave", rdbs_device_init);

INCLUDE_RODATA("asm/nonmatchings/libultra/bb/usb/rdb_slave", USB_STRING_DESC);

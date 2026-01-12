#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"

void __osBbVideoPllInit(s32 tvType);
void osBbPowerOff(void);

void func_80003A28(void*);

void func_80002248(void*);
void func_8000234C(void*);

void func_80003094(void);

extern u8 framebuffer[];
extern u32 D_8001AF40;
extern u32 D_800163A0;
extern u32 D_80016398;
extern u8 D_8001B0F8[0x8000];
extern u8 D_800232A8[0x8000];
extern OSThread D_8001AF48;
extern OSMesg prenmiMesgBuf[1];
extern OSMesgQueue prenmiMesgQueue;
extern OSThread D_800230F8;
extern OSMesgQueue piMesgQueue;
extern OSMesgQueue siMesgQueue;
extern OSMesgQueue siMesgQueue;
extern OSMesgQueue viMesgQueue;
extern OSMesg viMesgBuf[1];
extern OSMesg piMesgBuf[200];
extern OSMesg siMesgBuf[200];
extern OSMesg viRetraceMesg;
extern OSThread D_8002B2A8;
extern u8 D_8002B458[0x8000];
extern u32 D_80016390;
extern u32 D_80016394;
extern u32 D_80016398;
extern u32 D_8001639C;
extern s32 D_80016490;
extern s32 D_800164A0;
extern s32 D_800178A0;
extern s32 D_800178B0;

typedef struct {
    u8 data[0x100];
} Struct_Unk;

extern Struct_Unk D_80033458;

void func_80002050(u32 arg0) {
    D_80033458 = *(Struct_Unk*)PHYS_TO_K1(0x04A80100);
    func_80003094();
}

void func_80002114(s32 arg0) {
    D_8001AF40 = arg0;

    IO_WRITE(MI_3C_REG, 0x01000000);

    if (!(arg0 & 0x4C)) {
        __osBbVideoPllInit(OS_TV_NTSC);
        func_80003A28(framebuffer);
    } else {
        IO_WRITE(VI_H_VIDEO_REG, 0);
    }
    osInitialize();
    osCreateThread(&D_8001AF48, 1, func_80002248, 0, D_8001B0F8 + sizeof(D_8001B0F8), 0x14);
    osStartThread(&D_8001AF48);
}

void func_800021C8(void) {
    if (D_800163A0 != 0) {
        if (osRecvMesg(&prenmiMesgQueue, NULL, OS_MESG_NOBLOCK) == 0) {
            osBbPowerOff();
        }
    } else if (!(IO_READ(MI_38_REG) & 0x01000000)) {
        IO_WRITE(MI_3C_REG, 0x02000000);
        D_800163A0 = 1;
    }
}

void func_80002248(void* arg0) {
    osCreatePiManager(0x96, &piMesgQueue, piMesgBuf, ARRLEN(piMesgBuf));
    osCreateMesgQueue(&siMesgQueue, siMesgBuf, ARRLEN(siMesgBuf));
    osSetEventMesg(OS_EVENT_SI, &siMesgQueue, (OSMesg)200);
    osCreateThread(&D_800230F8, 3, func_8000234C, arg0, D_800232A8 + sizeof(D_800232A8), 0x12);
    osStartThread(&D_800230F8);
    osCreateMesgQueue(&prenmiMesgQueue, prenmiMesgBuf, ARRLEN(prenmiMesgBuf));
    osSetEventMesg(OS_EVENT_PRENMI, &prenmiMesgQueue, (OSMesg)1);
    osSetThreadPri(NULL, 0);

    while (TRUE) {
        ;
    }
}

void func_80002320(void) {
    while (TRUE) {
        osRecvMesg(&viMesgQueue, NULL, OS_MESG_BLOCK);
        func_800021C8();
    }
}

#ifdef NON_MATCHING
// Needs data imported
void func_8000234C(void* arg0) {
    s32 temp_s3;
    u64 temp_s0;
    u32 temp_s2;

    osCreateViManager(0xFE);
    fbInit(0);
    osCreateMesgQueue(&viMesgQueue, viMesgBuf, 1);
    osViSetEvent(&viMesgQueue, viRetraceMesg, 1);
    osViBlack(1);
    osViSwapBuffer(framebuffer);
    fbSetBg(0);
    fbClear();
    osViBlack(0);
    osWritebackDCacheAll();
    osViSwapBuffer(framebuffer);
    fbClear();
    func_80003C20();
    func_80002050(D_8001AF40);
    osCreateThread(&D_8002B2A8, 5, func_80002320, arg0, D_8002B458 + sizeof(D_8002B458), 0xF);
    osStartThread(&D_8002B2A8);
    D_80016398 = 1;
    osBbUsbSetCtlrModes(D_80016398 ^ 1, 0);
    osBbUsbSetCtlrModes(D_80016398 ^ 0, 2);
    osBbUsbInit();
    temp_s3 = func_80002DCC();

    D_80016390 = (D_80016398 == 0) ? !(IO_READ(0x04900018) & (1 << 7)) : 0;

    D_80016394 = !(IO_READ((D_80016398 == 0) ? 0x04900018 : 0x04A00018) & (1 << 5));

    if (D_80016394 == 0) {
        D_80016398 = ~D_80016398 & 1;
        osBbUsbSetCtlrModes(D_80016398 ^ 1, 0);
        osBbUsbSetCtlrModes(D_80016398 ^ 0, 2);
        osBbUsbInit();

        D_80016390 = (D_80016398 == 0) ? !(IO_READ(0x04900018) & (1 << 7)) : 0;
        D_80016394 = 0;
        temp_s2 = osBbUsbGetResetCount(D_80016398);
        temp_s0 = OS_CYCLES_TO_USEC(osGetCount()) + 2000000;
        while (OS_CYCLES_TO_USEC(osGetCount()) < temp_s0) {
            if (temp_s2 < osBbUsbGetResetCount(D_80016398)) {
                D_80016394 = 1;
                break;
            }
        }
    }

    D_8001639C = (D_80016390 != 0) || (D_80016394 == 0);
    osBbSetErrorLed(0);
    if (D_8001639C != 0) {
        if (temp_s3 != 0) {
            __osDisableInt();

            *(Struct_Unk*)PHYS_TO_K1(0x04A80100) = D_80033458;

            func_80002D94(temp_s3, D_8001AF40);
        } else {
            func_80004390(&D_800178B0, D_800178A0);
            func_800033CC();
        }
    } else {
        func_80004390(&D_800164A0, D_80016490);
        func_800033CC();
    }
    osBbPowerOff();
}
#else
INCLUDE_ASM("asm/nonmatchings/sa1/1050", func_8000234C);
#endif

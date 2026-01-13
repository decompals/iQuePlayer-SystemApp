#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"

void __osBbVideoPllInit(s32 tvType);
void osBbPowerOff(void);
void osBbUsbInit(void);
u32 osBbUsbGetResetCount(s32 which);
void osBbSetErrorLed(s32);
void __osDisableInt(void);
s32 osBbUsbSetCtlrModes(s32 which, u32 mask);

void fbInit(u32);
void fbSetBg(u16);
void fbClear(void);

void sa1InitRDP(void);
void sa1ConfigureVI(void*);
void sa1IdleThread(void*);
void sa1MainThread(void*);
s32 sa1CheckIdSys(void);
void sa1BootSA2(void (*entrypoint)(u32), u32 arg);
void* sa1AuthenticateAndLoadSA2(void);
s32 sa1DisplayImage(void*, size_t);
void sa1CommandLoop(void);

extern u8 framebuffer[];
extern OSMesg prenmiMesgBuf[1];
extern OSMesgQueue prenmiMesgQueue;
extern OSMesgQueue piMesgQueue;
extern OSMesgQueue siMesgQueue;
extern OSMesgQueue siMesgQueue;
extern OSMesgQueue viMesgQueue;
extern OSMesg viMesgBuf[1];
extern OSMesg piMesgBuf[200];
extern OSMesg siMesgBuf[200];
extern OSMesg viRetraceMesg;
extern OSThread idleThread;
extern u8 idleThreadStack[0x8000];
extern OSThread mainThread;
extern u8 mainThreadStack[0x8000];
extern OSThread eventThread;
extern u8 eventThreadStack[0x8000];

extern u32 skBootValue;
extern u32 powerBtnPressed;
extern u32 usbNum;
extern u32 usbTypeA;
extern u32 usbLineUnstable;
extern u32 usbNotConnected;
extern s32 sa1Image1Size;
extern s32 sa1Image1Data;
extern s32 sa1Image2Size;
extern s32 sa1Image2Data;
typedef struct {
    u8 data[0x100];
} Struct_Unk;
extern Struct_Unk appSavedState;

void sa1Setup(u32 arg0) {
    appSavedState = *(Struct_Unk*)PHYS_TO_K1(0x04A80100);
    sa1CheckIdSys();
}

void sa1Entrypoint(u32 arg) {
    skBootValue = arg;

    IO_WRITE(MI_3C_REG, 0x01000000);

    if (!(arg & 0x4C)) {
        // Cold boot
        __osBbVideoPllInit(OS_TV_NTSC);
        sa1ConfigureVI(framebuffer);
    } else {
        // Warm boot
        IO_WRITE(VI_H_VIDEO_REG, 0);
    }
    osInitialize();
    osCreateThread(&idleThread, 1, sa1IdleThread, 0, idleThreadStack + sizeof(idleThreadStack), 0x14);
    osStartThread(&idleThread);
}

void sa1Event(void) {
    if (powerBtnPressed) {
        if (osRecvMesg(&prenmiMesgQueue, NULL, OS_MESG_NOBLOCK) == 0) {
            osBbPowerOff();
        }
    } else if (!(IO_READ(MI_38_REG) & 0x01000000)) {
        IO_WRITE(MI_3C_REG, 0x02000000);
        powerBtnPressed = TRUE;
    }
}

void sa1IdleThread(void* arg) {
    osCreatePiManager(OS_PRIORITY_PIMGR, &piMesgQueue, piMesgBuf, ARRLEN(piMesgBuf));
    osCreateMesgQueue(&siMesgQueue, siMesgBuf, ARRLEN(siMesgBuf));
    osSetEventMesg(OS_EVENT_SI, &siMesgQueue, (OSMesg)200);
    osCreateThread(&mainThread, 3, sa1MainThread, arg, mainThreadStack + sizeof(mainThreadStack), 0x12);
    osStartThread(&mainThread);
    osCreateMesgQueue(&prenmiMesgQueue, prenmiMesgBuf, ARRLEN(prenmiMesgBuf));
    osSetEventMesg(OS_EVENT_PRENMI, &prenmiMesgQueue, (OSMesg)1);
    osSetThreadPri(NULL, 0);

    while (TRUE) {
        ;
    }
}

void sa1EventThread(void* arg) {
    while (TRUE) {
        osRecvMesg(&viMesgQueue, NULL, OS_MESG_BLOCK);
        sa1Event();
    }
}

#ifdef NON_MATCHING
// Needs data imported
void sa1MainThread(void* arg) {
    void* sa2Entrypoint;
    OSTime thresholdTime;
    u32 usbResetCount;

    osCreateViManager(OS_PRIORITY_VIMGR);
    fbInit(0);
    osCreateMesgQueue(&viMesgQueue, viMesgBuf, 1);
    osViSetEvent(&viMesgQueue, viRetraceMesg, 1);
    osViBlack(TRUE);
    osViSwapBuffer(framebuffer);
    fbSetBg(0);
    fbClear();
    osViBlack(FALSE);
    osWritebackDCacheAll();
    osViSwapBuffer(framebuffer);
    fbClear();
    sa1InitRDP();
    sa1Setup(skBootValue);
    osCreateThread(&eventThread, 5, sa1EventThread, arg, eventThreadStack + sizeof(eventThreadStack), 0xF);
    osStartThread(&eventThread);

    // Start with USB1
    usbNum = 1;
    osBbUsbSetCtlrModes(usbNum ^ 1, 0);
    osBbUsbSetCtlrModes(usbNum ^ 0, 2);
    osBbUsbInit();

    sa2Entrypoint = sa1AuthenticateAndLoadSA2();

    // No ID pin on USB1, always assume Type B / Unconnected
    usbTypeA = (usbNum == 0) ? !(IO_READ(0x04900018) & (1 << 7)) : 0;
    // Line state is unstable
    usbLineUnstable = !(IO_READ((usbNum == 0) ? 0x04900018 : 0x04A00018) & (1 << 5));

    if (!usbLineUnstable) {
        // If USB1 line state stable, it isn't connected?

        // Flip to USB0
        usbNum = ~usbNum & 1;
        osBbUsbSetCtlrModes(usbNum ^ 1, 0);
        osBbUsbSetCtlrModes(usbNum ^ 0, 2);
        osBbUsbInit();

        // USB0 has ID pin detection, use it to differentiate Type A or Type B / Unconnected
        usbTypeA = (usbNum == 0) ? !(IO_READ(0x04900018) & (1 << 7)) : 0;
        // Determine line stability by reset count over the next 2 seconds
        usbLineUnstable = 0;
        usbResetCount = osBbUsbGetResetCount(usbNum);
        thresholdTime = OS_CYCLES_TO_USEC(osGetCount()) + 2000000; // +2 seconds
        while (OS_CYCLES_TO_USEC(osGetCount()) < thresholdTime) {
            if (osBbUsbGetResetCount(usbNum) > usbResetCount) {
                usbLineUnstable = 1;
                break;
            }
        }
    }

    // Type A or stable
    usbNotConnected = usbTypeA || !usbLineUnstable;
    osBbSetErrorLed(0);
    if (usbNotConnected) {
        if (sa2Entrypoint != NULL) {
            // No USB, SA2
            __osDisableInt();

            *(Struct_Unk*)PHYS_TO_K1(0x04A80100) = appSavedState;

            sa1BootSA2(sa2Entrypoint, skBootValue);
        } else {
            // No USB, no SA2
            sa1DisplayImage(&sa1Image2Data, sa1Image2Size);
            sa1CommandLoop();
        }
    } else {
        // USB
        sa1DisplayImage(&sa1Image1Data, sa1Image1Size);
        sa1CommandLoop();
    }
    osBbPowerOff();
}
#else
INCLUDE_ASM("asm/nonmatchings/sa1/1050", sa1MainThread);
#endif

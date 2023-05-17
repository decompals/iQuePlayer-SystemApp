#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"

void __osBbVideoPllInit(s32 tvType);
void osBbPowerOff(void);

void func_80003A28(void*);

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

void func_80002248(void*);
void func_8000234C(void*);

void func_80003094(void);

typedef struct {
    u8 data[0x100];
} Struct_Unk;

extern Struct_Unk D_80033458;

void func_80002050(void) {
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

INCLUDE_ASM("asm/nonmatchings/sa1/1050", func_8000234C);

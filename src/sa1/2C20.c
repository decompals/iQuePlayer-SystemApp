#include "ultra64.h"
#include "sa1.h"
#include "macros.h"
#include "include_asm.h"

extern OSMesgQueue D_8006B770;
extern OSMesg D_8006B788[1];
extern OSMesg D_8006B78C;

typedef struct {
    Gfx cmdBuffer[0x2800];
    void* frameBuffer;
} GfxPool;

extern GfxPool D_80057768;

extern u8 D_8004FF60[];
extern u64 gfxYieldBuffer[OS_YIELD_DATA_SIZE/sizeof(u64)];
extern u64 gfxDramStack[0x400/sizeof(u64)];

extern void* D_8006B768;
extern u8 framebuffer[];

extern Gfx D_800163E0[];
extern Gfx D_80016408[];
extern Gfx* gfxListHead;

s32 expand_gzip(void* src, void* dst, size_t, size_t);



void func_80003C20(void) {
    osCreateMesgQueue(&D_8006B770, D_8006B788, ARRLEN(D_8006B788));
    osSetEventMesg(OS_EVENT_DP, &D_8006B770, D_8006B78C);
    D_80057768.frameBuffer = framebuffer;
}

void func_80003C7C(void* timg, s32 fmt, s32 siz, u32 width, u32 height, s32 tileWidthX, s32 tileWidthY, f32 x, f32 y) {
    f32 s;
    f32 t;
    s32 uls;
    s32 ult;
    s32 lrs;
    s32 lrt;
    s32 offx;
    s32 offy;
    s32 lrx;
    s32 lry;
    s32 ulx;
    s32 uly;
    f32 temp;

    gSPDisplayList(gfxListHead++, D_80016408);

    for (offx = 0; offx < width; offx += tileWidthX) {
        for (offy = 0; offy < height; offy += tileWidthY) {
            if (offx != 0) {
                s = (f32)offx - 0.5;
            } else {
                s = 0.5f;
            }

            if (offy != 0) {
                t = (f32)offy - 0.5;
            } else {
                t = 0.5f;
            }

            if (offx <= 0) {
                uls = 0;
            } else {
                uls = offx - 1;
            }

            if (offx + tileWidthX >= width) {
                lrs = width - 1;
            } else {
                lrs = offx + tileWidthX;
            }

            if (offy <= 0) {
                ult = 0;
            } else {
                ult = offy - 1;
            }

            if (offy + tileWidthY >= height) {
                lrt = height - 1;
            } else {
                lrt = offy + tileWidthY;
            }

            ulx = x + offx;
            lrx = ulx + tileWidthX + 1;
            temp = lrx;
            if (x + width < temp) {
                lrx = x + width;
            }

            uly = y + offy;
            lry = uly + tileWidthY + 1;
            temp = lry;
            if (y + height < temp) {
                lry = y + height;
            }

            gDPLoadTextureTile(gfxListHead++, timg, fmt, G_IM_SIZ_16b, width, height, uls, ult, lrs, lrt, 0,
                               G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                               G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSPTextureRectangle(gfxListHead++, ulx * 4, uly * 4, lrx * 4, lry * 4, G_TX_RENDERTILE,
                                (s32)(s * 32), (s32)(t * 32), 1 << 10, 1 << 10);
        }
    }
}

void func_800040EC(GfxPool* gfxPool) {
    OSTask gfxTask;

    gfxListHead = gfxPool->cmdBuffer;

    gSPDisplayList(gfxListHead++, D_80016408);
    gSPDisplayList(gfxListHead++, D_800163E0);
    gDPPipeSync(gfxListHead++);
    gDPSetCycleType(gfxListHead++, G_CYC_FILL);
    gDPSetColorImage(gfxListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, osVirtualToPhysical(gfxPool->frameBuffer));
    gDPSetFillColor(gfxListHead++, (GPACK_RGBA5551(0, 0, 0, 1) << 16) | GPACK_RGBA5551(0, 0, 0, 1));
    gDPFillRectangle(gfxListHead++, 0, 0, 320 - 1, 240 - 1);
    gDPPipeSync(gfxListHead++);

    func_80003C7C(D_8004FF60, G_IM_FMT_RGBA, G_IM_SIZ_16b, 160, 96, 32, 32, 80.0f, 72.0f);

    gDPFullSync(gfxListHead++);
    gSPEndDisplayList(gfxListHead++);

    gfxTask.t.type = M_GFXTASK;
    gfxTask.t.ucode_boot = rspbootTextStart;
	gfxTask.t.ucode_boot_size = (u32)rspbootTextEnd - (u32)rspbootTextStart;
	gfxTask.t.ucode = gspFast3DTextStart;
	gfxTask.t.ucode_data = gspFast3DDataStart;
	gfxTask.t.ucode_data_size = 0x800;
	gfxTask.t.dram_stack = gfxDramStack;
	gfxTask.t.dram_stack_size = 0x400;
	gfxTask.t.data_ptr = (u64*)gfxPool->cmdBuffer;
    gfxTask.t.flags = 0;
	gfxTask.t.output_buff = NULL;
	gfxTask.t.output_buff_size = NULL;
	gfxTask.t.data_size = (gfxListHead - gfxPool->cmdBuffer) * sizeof(Gfx);
	gfxTask.t.yield_data_ptr = gfxYieldBuffer;
	gfxTask.t.yield_data_size = OS_YIELD_DATA_SIZE;

    osWritebackDCacheAll();
    osSpTaskLoad(&gfxTask);
    osSpTaskStartGo(&gfxTask);
    osRecvMesg(&D_8006B770, &D_8006B78C, OS_MESG_BLOCK);
    osViSwapBuffer(gfxPool->frameBuffer);
}

s32 func_80004390(void* arg0, size_t arg1) {
    s32 ret;

    bzero(D_8004FF60, 0x7800);
    ret = expand_gzip(arg0, D_8004FF60, arg1, 0x7800);
    if (ret > 0) {
        func_800040EC(&D_80057768);
    }
    return ret;
}

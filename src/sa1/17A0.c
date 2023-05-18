#include "ultra64.h"
#include "PR/os_internal_reg.h"
#include "sa1.h"
#include "bbtypes.h"
#include "sha1.h"
#include "gcc/memory.h"
#include "macros.h"
#include "include_asm.h"

s32 osBbAtbSetup(u32 vAddrBase, u16* fsBlockList, u32 maxListLen);
extern u8 D_80033C10[0x4000];

s32 func_800027A0(s32 arg0, s32 arg1) {
    IO_WRITE(PI_70_REG, arg0 << 9);

    if (arg1 != 0) {
        IO_WRITE(PI_48_REG, 0x9F00CA10);
    } else {
        IO_WRITE(PI_48_REG, 0x9F008A10);
    }

    do {
        if (IO_READ(MI_38_REG) & 0x02000000) {
            IO_WRITE(PI_48_REG, 0);
            return -3;
        }
    } while (IO_READ(PI_48_REG) & 0x80000000);

    if (IO_READ(PI_48_REG) & 0x400) {
        return -2;
    }
    return 0;
}

u32 func_80002838(u32 regVal) {
    u32 temp_v1 = (regVal >> 8) & 0xFF;

    if (temp_v1 == ((regVal >> 0x10) & 0xFF)) {
        return temp_v1;
    }
    return regVal >> 0x18;
}

s32 func_80002860(s32 blockNum) {
    s32 ret;
    s32 var_a0;
    u32 var;
    s32 i;

    while (TRUE) {
        ret = func_800027A0(blockNum << 5, 0);
        if (ret == -3) {
            return -3;
        }
        var = IO_READ(PI_10404_REG);

        var_a0 = 0;
        for (i = 0; i < 8; i++) {
            if (!((var >> (16 + i)) & 1)) {
                var_a0++;
            }
        }

        blockNum++;
        if (var_a0 < 2) {
            break;
        }
    }
    if (ret == -2) {
        return -2;
    }
    return blockNum - 1;
}

s32 func_800028FC(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 0; i <= arg1; i++) {
        arg0 = func_80002860(arg0);

        if (arg0 < 0) {
            break;
        }
        arg0++;
    }
    return arg0 - 1;
}

s32 func_80002950(u8* arg0, void* arg1, s32 arg2) {
    SHA1Context sha1Ctx;
    BbShaHash hash;

    SHA1Reset(&sha1Ctx);
    SHA1Input(&sha1Ctx, arg1, arg2);
    SHA1Result(&sha1Ctx, (u8*)&hash);

    if (memcmp(&hash, arg0, sizeof(hash)) == 0) {
        return TRUE;
    }
    return FALSE;
}

s32 skVerifyHash(BbShaHash* hash, BbEccSig* outSignature, BbRsaCert**, BbAppLaunchCrls*);

s32 func_800029CC(u8* arg0) {
    BbRsaCert* certChain[3];
    BbShaHash hash;
    SHA1Context sha1Ctx;
    BbContentMetaDataHead* cmd = (BbContentMetaDataHead*)arg0;
    BbAppLaunchCrls* appLaunchCrls;

    certChain[0] = (BbRsaCert*)(cmd + 1);
    certChain[1] = (BbRsaCert*)(certChain[0] + 1);
    certChain[2] = NULL;

    appLaunchCrls = (BbAppLaunchCrls*)(arg0 + sizeof(*cmd) + (ARRLEN(certChain) - 1) * sizeof(BbRsaCert));

#define RELOCATE_PTR(field) { (field) = (void*)((u8*)(field) + (u32)arg0); }(void)0

    if (appLaunchCrls->carl.head != NULL) {
        RELOCATE_PTR(appLaunchCrls->carl.head);
    }
    RELOCATE_PTR(appLaunchCrls->carl.list);
    RELOCATE_PTR(appLaunchCrls->carl.certChain[0]);
    RELOCATE_PTR(appLaunchCrls->carl.certChain[1]);

    if (appLaunchCrls->cprl.head != NULL) {
        RELOCATE_PTR(appLaunchCrls->cprl.head);
    }
    RELOCATE_PTR(appLaunchCrls->cprl.list);
    RELOCATE_PTR(appLaunchCrls->cprl.certChain[0]);
    RELOCATE_PTR(appLaunchCrls->cprl.certChain[1]);

    SHA1Reset(&sha1Ctx);
    SHA1Input(&sha1Ctx, (void*)cmd, sizeof(BbContentMetaDataHead) - sizeof(BbRsaSig2048));
    SHA1Result(&sha1Ctx, (u8*)&hash);

    return skVerifyHash(&hash, &cmd->contentMetaDataSign, certChain, appLaunchCrls);
}

void* func_80002ABC(BbContentMetaDataHead* cmd, u16* fsBlockList, s32 maxListLen, void* dst) {
    OSMesgQueue retQueue;
    OSIoMesg mb;
    OSMesg retMesgBuf[1];
    OSPiHandle* cartHandle;
    u32 nBytes = maxListLen * 0x4000;

    osCreateMesgQueue(&retQueue, retMesgBuf, ARRLEN(retMesgBuf));

    if (maxListLen >= 0) {
        fsBlockList[maxListLen] = NULL;
        if (osBbAtbSetup(0x10000000, fsBlockList, maxListLen + 1) < 0) {
            return NULL;
        }
    }

    cartHandle = osCartRomInit();

    IO_WRITE(PI_48_REG, 0x1F008BFF);

    mb.hdr.pri = OS_MESG_PRI_NORMAL;
    mb.hdr.retQueue = &retQueue;
    mb.dramAddr = dst;
    mb.devAddr = 0;
    mb.size = nBytes;

    osEPiStartDma(cartHandle, &mb, OS_READ);
    osRecvMesg(&retQueue, NULL, OS_MESG_BLOCK);
    osWritebackDCacheAll();
    osInvalICache((void*)K0BASE, 0x10000);

    osRomBase = (void*)PHYS_TO_K1(PI_DOM1_ADDR2);
    osMemSize = 0x400000;
    osTvType = OS_TV_NTSC;

    if (cmd != NULL) {
        if (!func_80002950((u8*)cmd->hash, dst, cmd->size)) {
            return NULL;
        }
    }
    return dst;
}

s32 expand_gzip(void* src, void* dst, size_t, size_t);

void* func_80002BF8(BbContentMetaDataHead* cmd, u16* fsBlockList, s32 maxListLen, s32 arg3) {
    u32 temp_s0;
    u32 var_a0;
    u32 var_a2;

    if (func_80002ABC(cmd, fsBlockList, maxListLen, (void*)0x80200000) == NULL) {
        return NULL;
    }

    var_a2 = expand_gzip((void*)0x80200000, (void*)0x80300000, cmd->size, 0x100000);
    if (var_a2 < 0x1000) {
        return NULL;
    }

    temp_s0 = *(u32*)0x80300008;

    if (arg3 != 0) {
        var_a0 = temp_s0 - 0x1000;
        if (var_a0 < 0x80400000) {
            return NULL;
        }
        memcpy((void*)var_a0, (void*)0x80300000, var_a2);
    } else {
        if (temp_s0 < 0x80400000) {
            return NULL;
        }
        memcpy((void*)temp_s0, (void*)0x80301000, var_a2 - 0x1000);
    }
    return (void*)temp_s0;
}

s32 func_80002CB0(s32* arg0, u32 arg1) {
    s32 temp_s3;
    s32 i;
    s32 j;
    u32* var_s1 = (u32*)&D_80033C10;
    s32 ret;

    ret = func_800028FC(*arg0, arg1);
    if (ret < 0) {
        return ret;
    }

    temp_s3 = ret;

    for (i = 0; i < 0x20; i++) {
        ret = func_800027A0((temp_s3 << 5) + i, 0);
        if (ret < 0) {
            return ret;
        }

        if (i == 0) {
            *arg0 = func_80002838(IO_READ(PI_BASE_REG + 0x10400));
        }

        for (j = 0; j < 0x200; j += 4) {
            *var_s1++ = IO_READ(PI_BASE_REG + 0x10000 + j);
        }
    }

    if (func_800029CC(D_80033C10) == 0) {
        return 0;
    }
    return -1;
}

void func_80002D94(void (*arg0)(), void* arg1) {
    __osDisableInt();
    arg0(arg1);
}

extern u16 D_80037C10[72];

void* func_80002DCC(void) {
    u32 sp10 = 0;
    u32 sp14 = 0;
    void* var_v0;
    u32 temp_s1;
    u32 temp_s2;
    BbContentMetaDataHead* cmd = (BbContentMetaDataHead*)&D_80033C10[0];
    u32 i;

    if (func_80002CB0(&sp10, 4) < 0) {
        return 0;
    }
    sp14 = sp10;

    temp_s1 = cmd->size >> 0xE;

    for (i = 1; i <= temp_s1; i++) {
        if (func_800027A0(sp14 << 5, 0) < 0) {
            return 0;
        }
        sp14 = func_80002838(IO_READ(PI_BASE_REG + 0x10400));
    }

    if (func_80002CB0(&sp14, 0) < 0) {
        return 0;
    }

    temp_s2 = cmd->size >> 0xE;
    if (temp_s2 > 0x40) {
        return 0;
    }

    D_80037C10[0] = sp14;

    for (i = 1; i < temp_s2; i++) {
        if (func_800027A0(D_80037C10[i - 1] << 5, 0) < 0) {
            return 0;
        }
        D_80037C10[i] = func_80002838(IO_READ(PI_BASE_REG + 0x10400));
    }

    D_80037C10[i] = 0;

    var_v0 = func_80002BF8(cmd, D_80037C10, temp_s2, 1);
    if (var_v0 == NULL) {
        return NULL;
    }
    return var_v0;
}

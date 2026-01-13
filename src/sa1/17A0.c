#include "ultra64.h"
#include "PR/os_internal_reg.h"
#include "sa1.h"
#include "bbtypes.h"
#include "sha1.h"
#include "gcc/memory.h"
#include "macros.h"
#include "include_asm.h"

s32 osBbAtbSetup(u32 vAddrBase, u16* fsBlockList, u32 maxListLen);
s32 expand_gzip(void* src, void* dst, size_t, size_t);
s32 skVerifyHash(BbShaHash* hash, u32* outSignature, BbRsaCert**, BbAppLaunchCrls*);

extern u16 sa2BlockList[72];
extern u8 cmdBlock[0x4000];

s32 sa1CardReadPage(s32 pageNum, s32 bufSelect) {
    IO_WRITE(PI_70_REG, pageNum * 0x200);

    if (bufSelect != 0) {
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

/* Determines block link by majority vote */
u32 sa1NextGoodBlock(u32 curSpare) {
    u32 blockNum = (curSpare >> 8) & 0xFF;

    if (blockNum == ((curSpare >> 0x10) & 0xFF)) {
        return blockNum;
    }
    return curSpare >> 0x18;
}

s32 sa1CardReadBlock(s32 blockNum) {
    s32 ret;
    s32 var_a0;
    u32 var;
    s32 i;

    while (TRUE) {
        ret = sa1CardReadPage(blockNum * 0x20, 0);
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

s32 sa1SkipBlocks(s32 arg0, s32 nSkip) {
    s32 i;

    for (i = 0; i <= nSkip; i++) {
        arg0 = sa1CardReadBlock(arg0);

        if (arg0 < 0) {
            break;
        }
        arg0++;
    }
    return arg0 - 1;
}

s32 sa1CheckHash(u8* expectedHash, void* data, s32 dataSize) {
    SHA1Context sha1Ctx;
    BbShaHash hash;

    SHA1Reset(&sha1Ctx);
    SHA1Input(&sha1Ctx, data, dataSize);
    SHA1Result(&sha1Ctx, (u8*)&hash);

    if (memcmp(&hash, expectedHash, sizeof(hash)) == 0) {
        return TRUE;
    }
    return FALSE;
}

s32 sa1VerifyCMD(u8* arg0) {
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

    return skVerifyHash(&hash, cmd->contentMetaDataSign, certChain, appLaunchCrls);
}

void* sa1LoadAndCheckSA2(BbContentMetaDataHead* cmd, u16* fsBlockList, s32 maxListLen, void* dst) {
    OSMesgQueue retQueue;
    OSIoMesg mb;
    OSMesg retMesgBuf[1];
    OSPiHandle* cartHandle;
    u32 nBytes = maxListLen * 0x4000;

    osCreateMesgQueue(&retQueue, retMesgBuf, ARRLEN(retMesgBuf));

    if (maxListLen >= 0) {
        fsBlockList[maxListLen] = NULL;
        if (osBbAtbSetup(PI_DOM1_ADDR2, fsBlockList, maxListLen + 1) < 0) {
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
        if (!sa1CheckHash((u8*)cmd->hash, dst, cmd->size)) {
            return NULL;
        }
    }
    return dst;
}

void* sa1LoadAndDecompressSA2(BbContentMetaDataHead* cmd, u16* fsBlockList, s32 maxListLen, s32 includeHeader) {
    void* entrypoint;
    void* start;
    u32 size;

    if (sa1LoadAndCheckSA2(cmd, fsBlockList, maxListLen, (void*)0x80200000) == NULL) {
        return NULL;
    }

    size = (u32)expand_gzip((void*)0x80200000, (void*)0x80300000, cmd->size, 0x100000);
    if (size < 0x1000) {
        return NULL;
    }

    entrypoint = *(void**)0x80300008;

    if (includeHeader) {
        start = (void*)((u32)entrypoint - 0x1000);
        if (start < (void*)0x80400000) {
            return NULL;
        }
        memcpy(start, (void*)0x80300000, size);
    } else {
        if (entrypoint < (void*)0x80400000) {
            return NULL;
        }
        memcpy(entrypoint, (void*)0x80301000, size - 0x1000);
    }
    return entrypoint;
}

s32 sa1ReadAndVerifyCMD(u32* blockP, u32 numBlocksToSkip) {
    s32 blockNum;
    s32 i;
    s32 j;
    u32* dst = (u32*)&cmdBlock;
    s32 ret;

    // Skip arg1 bad blocks?
    ret = sa1SkipBlocks(*blockP, numBlocksToSkip);
    if (ret < 0) {
        return ret;
    }

    blockNum = ret;

    // Read 1 block's worth of data one page at a time
    for (i = 0; i < 0x20; i++) {
        ret = sa1CardReadPage(blockNum * 0x20 + i, 0);
        if (ret < 0) {
            return ret;
        }

        if (i == 0) {
            *blockP = sa1NextGoodBlock(IO_READ(PI_10400_REG));
        }

        for (j = 0; j < 0x200; j += 4) {
            *dst++ = IO_READ(PI_10000_BUF(j));
        }
    }

    if (sa1VerifyCMD(cmdBlock) == 0) {
        return 0;
    }
    return -1;
}

void sa1BootSA2(void (*entrypoint)(u32), u32 arg) {
    __osDisableInt();
    entrypoint(arg);
}

void* sa1AuthenticateAndLoadSA2(void) {
    u32 sp10 = 0;
    u32 sp14 = 0;
    void* entrypoint;
    u32 numSA1Blocks;
    u32 numSA2Blocks;
    BbContentMetaDataHead* cmd = (BbContentMetaDataHead*)&cmdBlock[0];
    u32 i;

    // Read CMD of SA1
    if (sa1ReadAndVerifyCMD(&sp10, 4) < 0) {
        return 0;
    }
    sp14 = sp10;

    numSA1Blocks = cmd->size / 0x4000;

    for (i = 1; i <= numSA1Blocks; i++) {
        if (sa1CardReadPage(sp14 * 0x20, 0) < 0) {
            return 0;
        }
        sp14 = sa1NextGoodBlock(IO_READ(PI_10400_REG));
    }

    // Read CMD of SA2
    if (sa1ReadAndVerifyCMD(&sp14, 0) < 0) {
        return 0;
    }

    numSA2Blocks = cmd->size / 0x4000;
    if (numSA2Blocks > 64) { // Max size of 64 blocks or 1MiB
        return 0;
    }

    sa2BlockList[0] = sp14;

    for (i = 1; i < numSA2Blocks; i++) {
        if (sa1CardReadPage(sa2BlockList[i - 1] * 0x20, 0) < 0) {
            return 0;
        }
        sa2BlockList[i] = sa1NextGoodBlock(IO_READ(PI_10400_REG));
    }

    sa2BlockList[i] = 0;

    entrypoint = sa1LoadAndDecompressSA2(cmd, sa2BlockList, numSA2Blocks, TRUE);
    if (entrypoint == NULL) {
        return NULL;
    }
    return entrypoint;
}

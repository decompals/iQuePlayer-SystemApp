#include "ultra64.h"
#include "sa1.h"
#include "PR/bb_fs.h"
#include "bbtypes.h"
#include "macros.h"
#include "include_asm.h"

s32 skGetId(BbId* bbId);

void osBbSetErrorLed(s32);
void __osBbDelay(u32);

void osBbCardInit(void);
s32 osBbCardReadBlock(u32 dev, u16 block, void* addr, void* spare);
s32 osBbCardEraseBlock(u32 dev, u16 block);
s32 osBbCardWriteBlock(u32 dev, u16 block, void* addr, void* spare);
s32 osBbCardStatus(u32 dev, u8* status);
s32 osBbCardChange(void);
s32 osBbCardClearChange(void);
u32 osBbCardBlocks(u32 dev);

s32 osBbReadHost(void*, u32);
s32 osBbWriteHost(void*, u32);

void osBbRtcInit(void);
void osBbRtcSet(u8 year, u8 month, u8 day, u8 dow, u8 hour, u8 min, u8 sec);
void osBbRtcGet(u8* year, u8* month, u8* day, u8* dow, u8* hour, u8* min, u8* sec);

s32 skSignHash(BbShaHash* hash, BbEccSig* outSignature);

extern OSThread D_80047DB0;
extern s32 D_800163C4;
extern u8 D_80047F60[0x8000];
extern OSMesgQueue D_800C9C28;
extern OSMesg D_8001AEB0;
extern OSBbFs D_80037CA0;
extern u8 D_8003FCA0[0x4000];
extern s8* D_800163C8;
extern s32 D_8001AE38; // card changed
extern s32 D_8001AEC4;
extern u32 D_8001AEC8;
extern OSBbFs D_80037CA0;
extern u8 D_8003FCA0[0x4000];
extern u8 D_80043CA0[0x4000];
extern u8 D_80047CA0[0x10];
extern u8 D_80047CB0[0x100];
extern s32 __osBbIsBb;

void func_80002F40(u32 delay) {
    osBbSetErrorLed(1);
    __osBbDelay(delay);
    osBbSetErrorLed(0);
    __osBbDelay(delay);
}

void func_80002F80(void* arg) {
    OSTimer timer;
    s32 var_s1 = 1;

    while (TRUE) {
        osBbSetErrorLed(var_s1);
        osSetTimer(&timer, 23437500, 0, &D_800C9C28, NULL);
        osRecvMesg(&D_800C9C28, NULL, OS_MESG_BLOCK);
        var_s1 ^= 1;
    }
}

void func_80002FE8(void) {
    if (D_800163C4 != 0) {
        osCreateMesgQueue(&D_800C9C28, &D_8001AEB0, 1);
        osCreateThread(&D_80047DB0, 9, func_80002F80, NULL, D_80047F60 + sizeof(D_80047F60), 9);
        D_800163C4 = 0;
    }
    osStartThread(&D_80047DB0);
}

void func_80003068(void) {
    osStopThread(&D_80047DB0);
    osBbSetErrorLed(0);
}

s32 func_80003094(void) {
    char sp10[9] = "temp.tmp";
    char sp20[7] = "id.sys";
    BbId bbId;
    s32 temp_s0;
    s32 fd;
    s32 ret;

    ret = skGetId(&bbId);
    if (ret < 0) {
        ret = -1;
        goto end;
    }

    if (osBbFInit(&D_80037CA0) < 0) {
        ret = -2;
        goto end;
    }

    bzero(D_8003FCA0, sizeof(D_8003FCA0));

    fd = osBbFOpen(sp20, "r");
    if (fd >= 0) {
        if (osBbFRead(fd, 0, &D_8003FCA0, sizeof(D_8003FCA0)) < 0) {
            osBbFClose(fd);
            ret = -2;
        } else {
            temp_s0 = *(u32*)&D_8003FCA0[0];

            osBbFClose(fd);

            if (bbId != temp_s0) {
                ret = -3;
            }
        }
    } else if (fd == -8) {
        bcopy(&bbId, &D_8003FCA0, sizeof(bbId));
        osBbFDelete(sp10);
        osBbFCreate(sp10, 1, sizeof(D_8003FCA0));

        fd = osBbFOpen(sp10, "w");
        if (fd < 0) {
            ret = -2;
        } else {
            if (osBbFWrite(fd, 0, &D_8003FCA0, sizeof(D_8003FCA0)) < 0) {
                osBbFClose(fd);
                osBbFDelete(sp10);
                ret = -2;
            }

            osBbFClose(fd);
            osBbFRename(sp10, sp20);
        }
    } else {
        ret = -2;
    }
end:
    return ret;
}

u32 func_8000328C(u8* data, s32 size, u32 initVal) {
    u32 csum = initVal;
    s32 i;

    for (i = 0; i < size; i++) {
        csum += data[i];
    }
    return csum;
}

// calculate checksum of "size" bytes of "fileName"
s32 func_800032B8(const char* fileName, u32 size, u32 expectedChksum) {
    OSBbStatBuf sb;
    s32 fd;
    s32 offset;
    u32 computedChksum = 0;
    s32 ret;
    u32 blockSize;
    s32 remaining;

    fd = osBbFOpen(fileName, "r");
    if (fd < 0) {
        return fd;
    }

    ret = osBbFStat(fd, &sb, NULL, 0);
    if (ret < 0) {
        return ret;
    }

    remaining = size;
    if (sb.size < remaining) {
        remaining = sb.size;
    }

    offset = 0;
    while (remaining > 0) {
        bzero(D_8003FCA0, sizeof(D_8003FCA0));
        osInvalDCache(&D_8003FCA0, sizeof(D_8003FCA0));
        blockSize = remaining;
        if (blockSize > sizeof(D_8003FCA0)) {
            blockSize = sizeof(D_8003FCA0);
        }
        ret = osBbFRead(fd, offset, &D_8003FCA0, sizeof(D_8003FCA0));
        if (ret < 0) {
            return ret;
        }
        remaining -= blockSize;
        offset += sizeof(D_8003FCA0);
        computedChksum = func_8000328C(D_8003FCA0, (s32)blockSize, computedChksum);
    }

    return (expectedChksum == computedChksum) ? 0 : -1;
}

typedef enum {
    CMD_WRITE_BLOCK = 6,
    CMD_READ_BLOCK = 7,

    CMD_NAND_BLOCK_STATS = 0xD,

    CMD_WRITE_BLOCK_WITH_SPARE = 0x10,
    CMD_READ_BLOCK_WITH_SPARE = 0x11,
    CMD_INIT_FS = 0x12,

    CMD_CARD_SIZE = 0x15,
    CMD_SET_SEQ_NUM = 0x16,
    CMD_GET_SEQ_NUM = 0x17,

    CMD_FILE_CHECKSUM = 0x1C,
    CMD_SET_LED = 0x1D,
    CMD_SET_TIME = 0x1E,
    CMD_GET_BBID = 0x1F,
    CMD_SIGN_HASH = 0x20,
} CmdId;

void func_800033CC(void) {
    u32 dataIn[2];
    u32 dataOut[2];
    u32 status[2];
    BbShaHash hash;
    BbEccSig eccSig;

    s32 numBlocks;
    u32 regVal;
    void* sparePtr;
    u32 ledValue;
    u32 length;
    u32 remaining;
    u32 n;
    s32 ret = 0;

    u8 year, month, day, dow, hour, min, sec;

    if (__osBbIsBb) {
        *D_800163C8 = 0;
        if (IO_READ(VIRAGE0_8000_REG) != 0) { // check for secure mode
            regVal = IO_READ(MI_14_REG);
            if (regVal & 0xFC) {
                for (n = 0; (s32)n < 6; n++) {
                    if ((regVal & (1 << (2 + n))) && n == 2) {
                        IO_WRITE(PI_STATUS_REG, PI_SET_RESET | PI_CLR_INTR);
                        IO_WRITE(PI_44_REG, 0);
                    }
                }
            }
        }

        if (__osBbIsBb) {
            osBbRtcInit();
            osBbCardInit();
            ret = osBbFInit(&D_80037CA0);
        }
    }

    D_8001AE38 = osBbCardClearChange();
    func_80002F40(100000);

    while (1) {
        if (ret < 0) {
            func_80003068();
        }

        ret = osBbReadHost(dataIn, sizeof(dataIn));
        if (ret < 0) {
            continue;
        }

        osBbCardStatus(0, (u8*)status);
        D_8001AEC4 = osBbCardChange();
        if (D_8001AEC4 != 0) {
            D_8001AEC8 = 0;
            D_8001AE38 = osBbCardClearChange();
            if (D_8001AE38) {
                osBbCardInit();
            }
        }

        dataOut[0] = 0xFF - dataIn[0];
        dataOut[1] = 0;
        sparePtr = NULL;

        switch (dataIn[0]) {
            case CMD_FILE_CHECKSUM:
                length = (dataIn[1] + 3) & ~3;
                if (length > sizeof(D_80047CB0)) {
                    length = sizeof(D_80047CB0);
                }
                ret = osBbReadHost(D_80047CB0, length);
                if (ret < 0) {
                    break;
                }
                D_80047CB0[0xFF] = 0;
                ret = osBbReadHost(dataIn, sizeof(dataIn));
                if (ret < 0) {
                    break;
                }
                dataOut[1] = func_800032B8(D_80047CB0, dataIn[1], dataIn[0]);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_SET_LED:
                func_80003068();
                ledValue = dataIn[1] & 3;
                if (ledValue < 2) {
                    osBbSetErrorLed(0);
                    dataOut[1] = 0;
                } else if (ledValue == 2) {
                    osBbSetErrorLed(1);
                    dataOut[1] = 0;
                } else if (ledValue != 3) {
                    dataOut[1] = 0;
                } else {
                    func_80002FE8();
                    dataOut[1] = 0;
                }
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_SET_TIME:
                dataOut[1] = 0;

                year = dataIn[1] >> 24 & 0xFF;
                month = dataIn[1] >> 16 & 0xFF;
                day = dataIn[1] >>  8 & 0xFF;
                dow = dataIn[1] >>  0 & 0xFF;

                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                if (ret < 0) {
                    break;
                }
                ret = osBbReadHost(dataIn, sizeof(dataIn[0]));
                if (ret < 0) {
                    break;
                }

                hour = dataIn[0] >> 16 & 0xFF;
                min = dataIn[0] >>  8 & 0xFF;
                sec = dataIn[0] >>  0 & 0xFF;

                osBbRtcSet(year, month, day, dow, hour, min, sec);
                break;

            case CMD_WRITE_BLOCK_WITH_SPARE:
                sparePtr = D_80047CA0;
            case CMD_WRITE_BLOCK:
                ret = osBbReadHost(D_8003FCA0, sizeof(D_8003FCA0));
                if (ret < 0) {
                    break;
                }
                if (sparePtr != NULL) {
                    ret = osBbReadHost(D_80047CA0, sizeof(D_80047CA0));
                    if (ret < 0) {
                        break;
                    }
                }
                osBbCardEraseBlock(0, dataIn[1]);
                dataOut[1] = osBbCardWriteBlock(0, dataIn[1], D_8003FCA0, sparePtr);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_READ_BLOCK_WITH_SPARE:
                sparePtr = D_80047CA0;
            case CMD_READ_BLOCK:
                dataOut[1] = osBbCardReadBlock(0, dataIn[1], D_8003FCA0, D_80047CA0);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                if (ret < 0) {
                    break;
                }
                ret = osBbWriteHost(D_8003FCA0, sizeof(D_8003FCA0));
                if (ret < 0) {
                    break;
                }
                if (sparePtr != NULL) {
                    ret = osBbWriteHost(D_80047CA0, sizeof(D_80047CA0));
                }
                break;

            case CMD_NAND_BLOCK_STATS:
                numBlocks = osBbCardBlocks(0);

                for (n = 0; n < numBlocks; n++) {
                    D_80047CA0[5] = 0xFF;
                    osBbCardReadBlock(0, n, D_8003FCA0, D_80047CA0);
                    D_80043CA0[n] = D_80047CA0[5] != 0xFF; // bad block indicator
                }
                dataOut[1] = n; // numBlocks
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                if (ret < 0) {
                    break;
                }
                ret = osBbWriteHost(D_80043CA0, n);
                break;

            case CMD_INIT_FS:
                dataOut[1] = osBbFInit(&D_80037CA0);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_CARD_SIZE:
                dataOut[1] = osBbCardBlocks(0);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_SET_SEQ_NUM:
                dataOut[1] = D_8001AEC8 = D_8001AE38 ? dataIn[1] : 0;
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_GET_SEQ_NUM:
                if (!D_8001AE38) {
                    dataOut[1] = -1;
                } else {
                    dataOut[1] = D_8001AEC8;
                }
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_GET_BBID:
                skGetId(&dataOut[1]);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_SIGN_HASH:
                n = dataIn[1];

                if (n == 0x14) {
                    ret = osBbReadHost(&hash, sizeof(hash));
                    if (ret < 0) {
                        break;
                    }
                    skSignHash(&hash, &eccSig);
                    dataOut[1] = 0x40;
                    ret = osBbWriteHost(dataOut, sizeof(dataOut));
                    if (ret  < 0) {
                        break;
                    }
                    ret = osBbWriteHost(&eccSig, sizeof(eccSig));
                    break;
                }

                dataOut[1] = -1;
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                if (ret < 0) {
                    break;
                }

                while ((s32)n > 0) {
                    remaining = MIN(n, 0x4000);
                    ret = osBbReadHost(D_8003FCA0, remaining);
                    if (ret < 0) {
                        break;
                    }
                    n -= remaining;
                }
                break;

            default:
                dataOut[1] = -1;
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;
        }
    }
}

#include "ultra64.h"
#include "sa1.h"
#include "PR/bb_fs.h"
#include "bbtypes.h"
#include "macros.h"
#include "include_asm.h"

s32 skGetId(BbId* bbId);
s32 skSignHash(BbShaHash* hash, BbEccSig* outSignature);

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

extern s32 __osBbIsBb;

extern OSThread ledThread;
extern OSMesgQueue ledTimerQueue;
extern OSMesg ledTimerMsgBuf;

extern s32 ledThreadNeedsInit; // = TRUE
extern u8 ledThreadStack[0x8000];
extern OSBbFs bbfs;
extern u8 blockData[0x4000];
extern u8 spareData[0x10];
extern s32 cardPresent;
extern s32 cardChanged;
extern u32 sequenceNumber;
extern u8 blockStats[0x4000];
extern char fileName[0x100];
extern s8* D_800163C8;

void sa1LEDBlinkOnce(u32 delay) {
    osBbSetErrorLed(1);
    __osBbDelay(delay);
    osBbSetErrorLed(0);
    __osBbDelay(delay);
}

void sa1LEDThread(void* arg) {
    OSTimer timer;
    s32 ledState = 1;

    while (TRUE) {
        osBbSetErrorLed(ledState);
        osSetTimer(&timer, OS_USEC_TO_CYCLES(500000), 0, &ledTimerQueue, NULL);
        osRecvMesg(&ledTimerQueue, NULL, OS_MESG_BLOCK);
        ledState ^= 1;
    }
}

void sa1LEDThreadStart(void) {
    if (ledThreadNeedsInit) {
        osCreateMesgQueue(&ledTimerQueue, &ledTimerMsgBuf, 1);
        osCreateThread(&ledThread, 9, sa1LEDThread, NULL, ledThreadStack + sizeof(ledThreadStack), 9);
        ledThreadNeedsInit = FALSE;
    }
    osStartThread(&ledThread);
}

void sa1LEDThreadStop(void) {
    osStopThread(&ledThread);
    osBbSetErrorLed(0);
}

s32 sa1CheckIdSys(void) {
    char temptmp[9] = "temp.tmp";
    char idsys[7] = "id.sys";
    BbId bbId;
    u32 savedBbId;
    s32 fd;
    s32 ret;

    ret = skGetId(&bbId);
    if (ret < 0) {
        ret = -1;
        goto end;
    }

    if (osBbFInit(&bbfs) < 0) {
        ret = -2;
        goto end;
    }

    bzero(blockData, sizeof(blockData));

    fd = osBbFOpen(idsys, "r");
    if (fd >= 0) {
        // id.sys exists ..
        if (osBbFRead(fd, 0, blockData, sizeof(blockData)) < 0) {
            // .. but cannot be read?
            osBbFClose(fd);
            ret = -2;
        } else {
            // .. check that the saved bbid is the same
            savedBbId = *(u32*)&blockData[0];

            osBbFClose(fd);

            if (bbId != savedBbId) {
                ret = -3;
            }
        }
    } else if (fd == -8) {
        // id.sys does not exist, create temp.tmp
        bcopy(&bbId, blockData, sizeof(bbId));
        osBbFDelete(temptmp);
        osBbFCreate(temptmp, 1, sizeof(blockData));

        // Open the created file
        fd = osBbFOpen(temptmp, "w");
        if (fd < 0) {
            ret = -2;
        } else {
            // Write the bbid
            if (osBbFWrite(fd, 0, blockData, sizeof(blockData)) < 0) {
                osBbFClose(fd);
                osBbFDelete(temptmp);
                ret = -2;
            }

            // Rename temp.tmp to id.sys
            osBbFClose(fd);
            osBbFRename(temptmp, idsys);
        }
    } else {
        // Other error when trying to open id.sys
        ret = -2;
    }
end:
    return ret;
}

u32 sa1CalcChecksum(u8* data, s32 size, u32 initVal) {
    u32 csum = initVal;
    s32 i;

    for (i = 0; i < size; i++) {
        csum += data[i];
    }
    return csum;
}

// calculate checksum of "size" bytes of "fileName"
s32 sa1VerifyChecksum(const char* fileName, u32 size, u32 expectedChksum) {
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
    if ((u32)remaining > sb.size) {
        remaining = sb.size;
    }

    offset = 0;
    while (remaining > 0) {
        bzero(blockData, sizeof(blockData));
        osInvalDCache(&blockData, sizeof(blockData));
        blockSize = remaining;
        if (blockSize > sizeof(blockData)) {
            blockSize = sizeof(blockData);
        }
        ret = osBbFRead(fd, offset, &blockData, sizeof(blockData));
        if (ret < 0) {
            return ret;
        }
        remaining -= blockSize;
        offset += sizeof(blockData);
        computedChksum = sa1CalcChecksum(blockData, (s32)blockSize, computedChksum);
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

void sa1CommandLoop(void) {
    u32 dataIn[2];
    u32 dataOut[2];
    u32 status[2];
    BbShaHash hash;
    BbEccSig eccSig;

    u32 numBlocks;
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
            ret = osBbFInit(&bbfs);
        }
    }

    cardPresent = osBbCardClearChange();
    sa1LEDBlinkOnce(100000);

    while (1) {
        if (ret < 0) {
            sa1LEDThreadStop();
        }

        ret = osBbReadHost(dataIn, sizeof(dataIn));
        if (ret < 0) {
            continue;
        }

        osBbCardStatus(0, (u8*)status);
        cardChanged = osBbCardChange();
        if (cardChanged) {
            sequenceNumber = 0;
            cardPresent = osBbCardClearChange();
            if (cardPresent) {
                osBbCardInit();
            }
        }

        dataOut[0] = 0xFF - dataIn[0];
        dataOut[1] = 0;
        sparePtr = NULL;

        switch (dataIn[0]) {
            case CMD_FILE_CHECKSUM:
                length = (dataIn[1] + 3) & ~3;
                if (length > sizeof(fileName)) {
                    length = sizeof(fileName);
                }
                ret = osBbReadHost(fileName, length);
                if (ret < 0) {
                    break;
                }
                fileName[255] = '\0';
                ret = osBbReadHost(dataIn, sizeof(dataIn));
                if (ret < 0) {
                    break;
                }
                dataOut[1] = sa1VerifyChecksum(fileName, dataIn[1], dataIn[0]);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_SET_LED:
                sa1LEDThreadStop();
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
                    sa1LEDThreadStart();
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
                sparePtr = spareData;
            case CMD_WRITE_BLOCK:
                ret = osBbReadHost(blockData, sizeof(blockData));
                if (ret < 0) {
                    break;
                }
                if (sparePtr != NULL) {
                    ret = osBbReadHost(spareData, sizeof(spareData));
                    if (ret < 0) {
                        break;
                    }
                }
                osBbCardEraseBlock(0, dataIn[1]);
                dataOut[1] = osBbCardWriteBlock(0, dataIn[1], blockData, sparePtr);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_READ_BLOCK_WITH_SPARE:
                sparePtr = spareData;
            case CMD_READ_BLOCK:
                dataOut[1] = osBbCardReadBlock(0, dataIn[1], blockData, spareData);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                if (ret < 0) {
                    break;
                }
                ret = osBbWriteHost(blockData, sizeof(blockData));
                if (ret < 0) {
                    break;
                }
                if (sparePtr != NULL) {
                    ret = osBbWriteHost(spareData, sizeof(spareData));
                }
                break;

            case CMD_NAND_BLOCK_STATS:
                numBlocks = osBbCardBlocks(0);

                for (n = 0; n < numBlocks; n++) {
                    spareData[5] = 0xFF;
                    osBbCardReadBlock(0, n, blockData, spareData);
                    blockStats[n] = spareData[5] != 0xFF; // bad block indicator
                }
                dataOut[1] = n; // numBlocks
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                if (ret < 0) {
                    break;
                }
                ret = osBbWriteHost(blockStats, n);
                break;

            case CMD_INIT_FS:
                dataOut[1] = osBbFInit(&bbfs);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_CARD_SIZE:
                dataOut[1] = osBbCardBlocks(0);
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_SET_SEQ_NUM:
                dataOut[1] = sequenceNumber = cardPresent ? dataIn[1] : 0;
                ret = osBbWriteHost(dataOut, sizeof(dataOut));
                break;

            case CMD_GET_SEQ_NUM:
                if (!cardPresent) {
                    dataOut[1] = -1;
                } else {
                    dataOut[1] = sequenceNumber;
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
                    ret = osBbReadHost(blockData, remaining);
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

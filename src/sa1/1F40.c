#include "ultra64.h"
#include "sa1.h"
#include "PR/bb_fs.h"
#include "bbtypes.h"
#include "macros.h"
#include "include_asm.h"

s32 skGetId(BbId* bbId);

void osBbSetErrorLed(s32);
void __osBbDelay(u32);

extern OSThread D_80047DB0;
extern s32 D_800163C4;
extern u8 D_80047F60[0x8000];
extern OSMesgQueue D_800C9C28;
extern OSMesg D_8001AEB0;

extern OSBbFs D_80037CA0;
extern u8 D_8003FCA0[0x4000];

void func_80002F40(s32 delay) {
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

#ifdef NON_EQUIVALENT
s32 func_80003094(void) {
    char sp10[16];
    char sp20[8];
    BbId bbId;
    s32 temp_s0;
    s32 fd;
    s32 ret;

    // This isn't correct
    __builtin_memcpy(sp10, "temp.tmp", 9);
    __builtin_memcpy(sp20, "id.sys", 7);

    ret = skGetId(&bbId);
    if (ret < 0) {
        return -1;
    }

    if (osBbFInit(&D_80037CA0) < 0) {
        return -2;
    }

    bzero(D_8003FCA0, sizeof(D_8003FCA0));

    fd = osBbFOpen(&sp20, "r");
    if (fd >= 0) {
        if (osBbFRead(fd, 0, &D_8003FCA0, sizeof(D_8003FCA0)) < 0) {
            osBbFClose(fd);
            return -2;
        }

        temp_s0 = *(u32*)&D_8003FCA0[0];

        osBbFClose(fd);

        if (bbId != temp_s0) {
            return -3;
        }
        return ret;
    }
    if (fd != -8) {
        return -2;
    }

    bcopy(&bbId, &D_8003FCA0, sizeof(bbId));
    osBbFDelete(&sp10);
    osBbFCreate(&sp10, 1, sizeof(D_8003FCA0));

    fd = osBbFOpen(&sp10, "w");
    if (fd < 0) {
        return -2;
    }

    if (osBbFWrite(fd, 0, &D_8003FCA0, sizeof(D_8003FCA0)) < 0) {
        osBbFClose(fd);
        osBbFDelete(&sp10);
        return -2;
    }

    osBbFClose(fd);
    osBbFRename(&sp10, &sp20);
    return ret;
}
#else
INCLUDE_RODATA("asm/nonmatchings/sa1/1F40", D_80019600);
INCLUDE_RODATA("asm/nonmatchings/sa1/1F40", D_8001960C);
extern const char D_80019614[];
INCLUDE_RODATA("asm/nonmatchings/sa1/1F40", D_80019614);
INCLUDE_ASM("asm/nonmatchings/sa1/1F40", func_80003094);
#endif

s32 func_8000328C(u8* arg0, s32 arg1, s32 arg2) {
    s32 var_a2 = arg2;
    s32 var_v1;

    for (var_v1 = 0; var_v1 < arg1; var_v1++) {
        var_a2 += arg0[var_v1];
    }
    return var_a2;
}

s32 func_800032B8(const char* arg0, u32 arg1, s32 arg2) {
    OSBbStatBuf sb;
    s32 fd;
    s32 var_s3;
    s32 var_s5;
    s32 ret;
    u32 var_s0;
    s32 remaining;

    var_s5 = 0;

    fd = osBbFOpen(arg0, D_80019614);
    if (fd < 0) {
        return fd;
    }

    ret = osBbFStat(fd, &sb, 0, 0);
    if (ret < 0) {
        return ret;
    }

    remaining = arg1;
    if (sb.size < remaining) {
        remaining = sb.size;
    }

    var_s3 = 0;
    while (remaining > 0) {
        bzero(&D_8003FCA0, 0x4000);
        osInvalDCache(&D_8003FCA0, 0x4000);
        var_s0 = remaining;
        if (var_s0 > 0x4000) {
            var_s0 = 0x4000;
        }
        ret = osBbFRead(fd, var_s3, &D_8003FCA0, 0x4000);
        if (ret < 0) {
            return ret;
        }
        remaining -= var_s0;
        var_s3 += 0x4000;
        var_s5 = func_8000328C(D_8003FCA0, var_s0, var_s5);
    }

    ret = -1;
    if (arg2 == var_s5) {
        ret = 0;
    }
    return ret;
}

INCLUDE_ASM("asm/nonmatchings/sa1/1F40", func_800033CC);

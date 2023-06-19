#ifndef _BB_FS_H_
#define _BB_FS_H_

#include "PR/ultratypes.h"

typedef struct {
    /* 0x0000 */ u8 root[32768];
} OSBbFs; // size = 0x8000

typedef struct {
    /* 0x0000 */ u16 files;
    /* 0x0002 */ u16 blocks;
    /* 0x0004 */ u16 freeFiles;
    /* 0x0006 */ u16 freeBlocks;
} OSBbStatFs; // size = 0x8

typedef struct {
    /* 0x0000 */ u8 type;
    /* 0x0004 */ u32 size;
} OSBbStatBuf; // size = 0x8

s32 osBbFOpen(const char* name, const char* mode);
s32 osBbFWrite(s32 fd, u32 off, void* buf, u32 len);
s32 osBbFRead(s32 fd, u32 off, void* buf, u32 len);
s32 osBbFClose(s32 fd);
s32 osBbFStatFs(OSBbStatFs* statfs);
s32 osBbFInit(OSBbFs* fs);
s32 osBbFDelete(const char* name);
s32 osBbFCreate(const char* name, u8 type, u32 len);
s32 osBbFRename(const char* old, const char* new);
s32 osBbFStat(s32 fd, OSBbStatBuf* sb, u16* blockList, u32 listLen);

#endif

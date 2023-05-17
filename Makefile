TARGET := build/1106_sa1.bin
ELF := $(TARGET:.bin=.elf)

COMPARE ?= 1

PYTHON := python3
SPLAT := $(PYTHON) tools/splat/split.py
ELFPATCH := $(PYTHON) tools/elfpatch.py

CROSS := mips-linux-gnu-

export COMPILER_PATH := tools/egcs/
CC      := $(COMPILER_PATH)gcc

AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
STRIP   := $(CROSS)strip

INC := -I include -I include/PR -I include/sys -I src
CFLAGS := $(INC) -D_MIPS_SZLONG=32 -D_LANGUAGE_C -DBBPLAYER -nostdinc -fno-builtin -fno-PIC -mno-abicalls -mcpu=4300 -G 0 -mgp32 -Wall -Wa,-Iinclude
ASFLAGS := $(INC) -D_MIPS_SZLONG=32 -D_LANGUAGE_ASSEMBLY -DBBPLAYER -nostdinc -fno-PIC -mno-abicalls -mcpu=4300 -G 0
OPTFLAGS := -Os -g3
MIPS_VERSION := -mips2

build/src/libultra/reg/_getcount.o: MIPS_VERSION := -mips3
build/src/libultra/reg/_setcompare.o: MIPS_VERSION := -mips3
build/src/libz/%.o: MIPS_VERSION := -mips3

# Source dirs
SRC_DIRS := $(shell find src -type d)
ASM_DIRS := $(shell find asm -type d -not -path "asm/nonmatchings*" -not -path "asm/data*")
DATA_DIRS := $(shell find asm/data -type d)
# Source files
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
S_FILES := $(foreach dir,$(SRC_DIRS) $(ASM_DIRS),$(wildcard $(dir)/*.s))
DATA_FILES := $(foreach dir,$(DATA_DIRS),$(wildcard $(dir)/*.s))
O_FILES := $(foreach f,$(C_FILES:.c=.o),build/$f) \
           $(foreach f,$(S_FILES:.s=.o),build/$f) \
		   $(foreach f,$(DATA_FILES:.s=.o),build/$f)

# Create build directories
$(shell mkdir -p build $(foreach dir,$(SRC_DIRS) $(ASM_DIRS) $(DATA_DIRS),build/$(dir)))

.PHONY: all clean distclean disasm setup

all: $(TARGET)
ifeq ($(COMPARE),1)
	@sha1sum $(TARGET)
	@sha1sum -c 1106_sa1.sha1
endif

clean:
	$(RM) -r build
	$(MAKE) -C tools clean

distclean: clean
	$(RM) -r assets asm
	$(RM) -r expected
	$(MAKE) -C tools distclean

disasm:
	$(RM) -r assets asm
	$(SPLAT) sa1.yaml --modes code data bin
	$(RM) asm/header.s

setup:
	$(MAKE) -C tools
	$(MAKE) disasm
	$(MAKE) all COMPARE=1
	mkdir -p expected && cp -r build expected/build

$(TARGET): $(ELF)
	$(OBJCOPY) -O binary $< $(@:.bin=.tmp)
	dd if=$(@:.bin=.tmp) of=$@ bs=16K conv=sync status=none
	@$(RM) $(@:.bin=.tmp)

$(ELF): $(O_FILES) sa1.lcf os_syms.txt
	$(LD) -T sa1.lcf -T os_syms.txt -o $@ -Map $(@:.elf=.map)

build/asm/%.o: asm/%.s
	$(AS) -march=vr4300 --no-pad-sections -I include $< -o $@
	$(ELFPATCH) $@

build/src/makerom/ipl3.o: src/makerom/ipl3.s
	$(AS) -march=vr4300 --no-pad-sections -I include $< -o $@
	$(ELFPATCH) $@

build/src/%.o: src/%.s
	$(CC) -x assembler-with-cpp $(ASFLAGS) $(MIPS_VERSION) -c $< -o $@
	@$(STRIP) -N dummy_symbol_ $@
	$(ELFPATCH) $@
	@$(OBJDUMP) -drz $@ > $(@:.o=.s)

build/src/%.o: src/%.c
	$(CC) $(CFLAGS) $(MIPS_VERSION) $(OPTFLAGS) -c $< -o $@
	@$(STRIP) -N dummy_symbol_ $@
	$(ELFPATCH) $@
	@$(OBJDUMP) -drz $@ > $(@:.o=.s)

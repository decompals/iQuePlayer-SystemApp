CC := gcc
CXX := g++
CFLAGS := -I. -Wall -Wextra -pedantic -std=c99 -g -O2
LDFLAGS := -lm
PROGRAMS := armips

EGCS_DIR := egcs
EGCS-AS := $(EGCS_DIR)/as
EGCS-1.1.2 := $(EGCS_DIR)/gcc

.PHONY: all clean distclean

all: $(EGCS-AS) $(EGCS-1.1.2) $(PROGRAMS)

clean:
	

distclean: clean
	$(RM) -rf $(GCC_DIR) $(EGCS_DIR)
	$(RM) $(PROGRAMS) $(addsuffix .exe,$(PROGRAMS))

.PHONY: all clean distclean

armips_SOURCES := armips.cpp

armips: CC := $(CXX)
armips_CFLAGS := -std=c++11 -fno-exceptions -fno-rtti -pipe -Wno-unused-parameter -Wno-maybe-uninitialized
armips_LDFLAGS := -pthread

define COMPILE
$(1): $($1_SOURCES)
	$$(CC) $(CFLAGS) $($1_CFLAGS) $$^ -o $$@ $($1_LDFLAGS) $(LDFLAGS)
endef

$(foreach p,$(PROGRAMS),$(eval $(call COMPILE,$(p))))

# EGCS

$(EGCS-AS): | $(EGCS_DIR)
	wget https://github.com/decompals/mips-binutils-egcs-2.9.5/releases/latest/download/mips-binutils-egcs-2.9.5-linux.tar.gz
	tar xf mips-binutils-egcs-2.9.5-linux.tar.gz -C $(EGCS_DIR)
	$(RM) mips-binutils-egcs-2.9.5-linux.tar.gz

$(EGCS-1.1.2): | $(EGCS_DIR)
	wget https://github.com/decompals/mips-gcc-egcs-2.91.66/releases/latest/download/mips-gcc-egcs-2.91.66-linux.tar.gz
	tar xf mips-gcc-egcs-2.91.66-linux.tar.gz -C $(EGCS_DIR)
	$(RM) mips-gcc-egcs-2.91.66-linux.tar.gz

$(EGCS_DIR):
	mkdir -p $@

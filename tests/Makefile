# =============================================================================
# Test Suite — Suture Tests (LibCPLTest)
# =============================================================================

ROOT := ..
include $(ROOT)/common.mk

.PHONY: all shellcode runner run clean

all: shellcode runner

libs: $(BIN)
	$(call build_core_libs)
	@$(CC_64) -DWIN_X64 $(CFLAGS) \
		-c $(AGENT_SRC)/modules.c -o $(BIN)/modules.x64.o
	@$(CC_64) -DWIN_X64 $(CFLAGS) \
		-c $(AGENT_LIBS)/LibCPLTest/src/cpltest.c -o $(BIN)/cpltest.x64.o
	$(call zip_mergelib,modules)
	$(call zip_mergelib,cpltest)
	@echo "    modules: $$(wc -c < $(BIN)/modules.x64.o)b  cpltest: $$(wc -c < $(BIN)/cpltest.x64.o)b"

objects: $(BIN)
	@echo "[1/4] Compiling test COFF objects"
	$(call build_loader)
	@$(CC_64) -DWIN_X64 -DCPLTESTS $(CFLAGS) -c pico_test.c -o $(BIN)/test_pico.x64.o
	@echo "    loader: $$(wc -c < $(BIN)/loader.x64.o)b  test_pico: $$(wc -c < $(BIN)/test_pico.x64.o)b"

dll: $(BIN)
	$(call build_stub)

shellcode: libs objects dll
	@echo "[3/4] Crystal Palace link (tests)"
	@$(CPL) link loader.spec $(BIN)/stub.x64.dll $(BIN)/tests.x64.bin
	@echo "    Test shellcode: $$(wc -c < $(BIN)/tests.x64.bin) bytes"

runner: $(BIN)
	$(call build_runner)

run: shellcode runner
	@echo "=== Running test suite ==="
	@$(BIN)/run.x64.exe $(BIN)/tests.x64.bin

clean:
	rm -f $(BIN)/test_pico.x64.o $(BIN)/tests.x64.bin
	rm -f $(BIN)/cpltest.x64.o $(BIN)/libcpltest.x64.zip
	rm -f $(BIN)/modules.x64.o $(BIN)/libmodules.x64.zip

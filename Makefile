# =============================================================================
# Suture
# =============================================================================
#
# A modular, position-independent C2 agent framework built on
# Crystal Palace (TCG) with DFR-based dynamic function resolution.
#
# Projects build against the core agent/ framework and produce
# standalone shellcode payloads via Crystal Palace linking.
#
# Usage:
#   make basic         — build the basic agent (connect + hello)
#   make demo          — build the demo agent (module framework)
#   make basic-run     — build + run basic
#   make demo-run      — build + run demo
#   make test          — build the test suite (LibCPLTest)
#   make test-run      — build + run the test suite
#   make server        — build Go PoC server
#   make clean         — clean all build artifacts
#
# New project:
#   1. Create a folder (e.g. myagent/)
#   2. Copy basic/ as a starting point
#   3. Add your agent logic + any library mergelibs
#   4. Add a target here: myagent: @$(MAKE) -C myagent shellcode runner
# =============================================================================

.PHONY: basic basic-run demo demo-run test test-run server clean

basic:
	@$(MAKE) -C basic shellcode runner

basic-run:
	@$(MAKE) -C basic run

basic-debug:
	@$(MAKE) -C basic debug

demo:
	@$(MAKE) -C demo shellcode runner

demo-run:
	@$(MAKE) -C demo run

demo-debug:
	@$(MAKE) -C demo debug

test:
	@$(MAKE) -C tests shellcode runner

test-run:
	@$(MAKE) -C tests run

server:
	@cd server && go build -o ../bin/server .

clean:
	rm -rf bin
	@$(MAKE) -C basic clean 2>/dev/null || true
	@$(MAKE) -C demo clean 2>/dev/null || true
	@$(MAKE) -C tests clean 2>/dev/null || true

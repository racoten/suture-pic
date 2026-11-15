#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "../agent/libs/LibDebug/src/debug.h"
#include "../agent/libs/LibUtils/src/utils.h"
#include "../agent/libs/LibCPLTest/src/cpltest.h"
#include "../agent/src/agent.h"

#include "test_utils.c"
#include "test_modules.c"
#include "../agent/src/free.c"

void agent_main(void) {
    dprintf("=== Suture Test Suite ===");

    TESTFUNCS tests = initTests();

    dprintf("--- Registering LibUtils tests ---");
    register_utils_tests(&tests);

    dprintf("--- Registering module dispatcher tests ---");
    register_modules_tests(&tests);

    dprintf("--- Running %i tests ---", (int)tests.size);
    runTests(&tests);

    dprintf("=== Test suite complete (%i tests executed) ===", (int)tests.size);
    freeTests(&tests);
}

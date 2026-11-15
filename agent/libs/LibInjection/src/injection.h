#ifndef INJECTION_H
#define INJECTION_H

#include <windows.h>
#include <stddef.h>

int inject_shellcode(DWORD pid, const void *shellcode, size_t sc_len);
int inject_apc(DWORD pid, DWORD tid, const void *shellcode, size_t sc_len);
int inject_thread_hijack(DWORD pid, DWORD tid, const void *shellcode, size_t sc_len);
int inject_earlybird(const char *target_exe, const void *shellcode, size_t sc_len);
int inject_module_stomp(DWORD pid, const char *dll_name, const void *shellcode, size_t sc_len);
int inject_spawn(const char *target_exe, const void *shellcode, size_t sc_len, DWORD *out_pid);

#endif

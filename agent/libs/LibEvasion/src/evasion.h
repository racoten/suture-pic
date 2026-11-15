#ifndef EVASION_H
#define EVASION_H

#include <windows.h>

int evasion_sleep(DWORD milliseconds);
int evasion_patch_etw(void);
int evasion_patch_amsi(void);
int evasion_unhook_ntdll(void);
int evasion_heap_encrypt(void);
int evasion_heap_decrypt(void);

#endif

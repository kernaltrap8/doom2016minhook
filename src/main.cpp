// doom2016minhook Copyright (C) 2025 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>

bool InjectDLL(DWORD pid, const std::string& dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) return false;

    LPVOID pRemote = VirtualAllocEx(hProcess, nullptr, dllPath.size() + 1,
                                    MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pRemote) {
        CloseHandle(hProcess);
        return false;
    }

    WriteProcessMemory(hProcess, pRemote, dllPath.c_str(), dllPath.size() + 1, nullptr);

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
                                        (LPTHREAD_START_ROUTINE)LoadLibraryA,
                                        pRemote, 0, nullptr);
    if (!hThread) {
        VirtualFreeEx(hProcess, pRemote, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemote, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return true;
}

int main(int argc, char **argv) {
    if (argc != 3) {
      fprintf(stderr, "need path to game exe and dll to inject\n");
      fprintf(stderr, "usage: %s [exePath] [dllPath]\n", argv[0]);
      exit(1);
    }

    const char *exePath = argv[1];
    const char *dllPath = argv[2];
    
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(exePath, nullptr, nullptr, nullptr, FALSE,
                        CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) {
        std::cerr << "Failed to start process.\n";
        return 1;
    }

    if (!InjectDLL(pi.dwProcessId, dllPath)) {
        std::cerr << "DLL injection failed.\n";
        TerminateProcess(pi.hProcess, 1);
        return 1;
    }

    // Resume Doom
    ResumeThread(pi.hThread);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}

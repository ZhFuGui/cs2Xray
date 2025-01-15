#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>
#include <iostream>
// Error code enumeration
enum class ErrorCode {
    SUCCESS = 1,
    PROCESS_NOT_FOUND,
    PROCESS_OPEN_FAILED,
    MODULE_NOT_FOUND,
    MEMORY_WRITE_FAILED
};

// Get process PID
DWORD GetProcessPidByName(WCHAR const* procName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap) {
        PROCESSENTRY32W entry;
        entry.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnap, &entry)) {
            do {
                if (!_wcsicmp(entry.szExeFile, procName)) {

                    CloseHandle(hSnap);
                    return entry.th32ProcessID;
                };

            } while (Process32NextW(hSnap, &entry));

        };
        CloseHandle(hSnap);
    };
    return 0;
};

// Get module handle
HMODULE GetModule(HANDLE hProcess, WCHAR const ModuleName[MAX_PATH]) {
    DWORD pro_base = NULL;
    HMODULE hModule[999] = { 0 };
    DWORD dwRet = 0;
    int num = 0;
    int bRet = EnumProcessModulesEx(hProcess, (HMODULE*)(hModule), sizeof(hModule), &dwRet, NULL);
    num = dwRet / sizeof(HMODULE);
    wchar_t lpBaseName[MAX_PATH];
    for (int i = 0; i < num + 1; i++) {
        GetModuleBaseNameW(hProcess, hModule[i], lpBaseName, sizeof(lpBaseName) / sizeof(WCHAR));
        if (!_wcsicmp(lpBaseName, ModuleName)) {
            return hModule[i];
        }
    }
    delete[] hModule;
    MessageBoxW(0, L"Module handle not obtained", L"Error", 0);
    return 0;
}

// Write memory
BOOL wMem(WCHAR const* ExeName, WCHAR const* ModuleName, uint64_t Offset, DWORD64 dwValue, DWORD dwLen) {
    DWORD dwPid = GetProcessPidByName(ExeName);
    if (dwPid == 0) {
        SetLastError((DWORD)ErrorCode::PROCESS_NOT_FOUND);
        return FALSE;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    if (hProcess == NULL) {
        SetLastError((DWORD)ErrorCode::PROCESS_OPEN_FAILED);
        return FALSE;
    }

    HMODULE ModuleAddress = 0;
    if (ModuleName != NULL) {
        ModuleAddress = GetModule(hProcess, ModuleName);
        if (ModuleAddress == NULL) {
            CloseHandle(hProcess);
            SetLastError((DWORD)ErrorCode::MODULE_NOT_FOUND);
            return FALSE;
        }
    }

    BOOL writeResult = WriteProcessMemory(hProcess, (LPVOID)((uint64_t)ModuleAddress + Offset), &dwValue, dwLen, 0);

    if (writeResult == FALSE) {
        CloseHandle(hProcess);
        SetLastError((DWORD)ErrorCode::MEMORY_WRITE_FAILED);
        return FALSE;
    }

    CloseHandle(hProcess);
    return TRUE;
};

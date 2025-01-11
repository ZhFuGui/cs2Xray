#include <iostream>
#include <Windows.h>
#include <math.h>
#include <TlHelp32.h>
#include <string.h>
#include <tchar.h>
#include <psapi.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <future>
#include <unordered_map>
#include "SignatureLocation.h"
#include "FileOperations.h"
#include "MemoryReadWrite.h"
#include "Miscellaneous.h"

uint64_t Offset = 0;
uint64_t Tem_Offset = 0;
BOOL XrayOpen = 3;

// Output feature code to get offset
BOOL GetCs2XrayOffset(DWORD64& Offset, char* code) {

    DWORD GamePid = GetProcessPidByName(L"cs2.exe");
    if (!GamePid) { return 0; }
    HANDLE GameProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GamePid);
    HMODULE ClientModuleAddress = GetModule(GameProcess, L"client.dll");

    DWORD64 addr = 0;

    scanGameCode(GameProcess, (uint64_t)ClientModuleAddress + 0x0, (uint64_t)ClientModuleAddress + 0x1000000, code, strlen(code), addr);

    if (!addr) {
        Offset = 0;
        CloseHandle(GameProcess);
        return FALSE;
    }

    Offset = addr - (uint64_t)ClientModuleAddress;
    CloseHandle(GameProcess);
    return TRUE;
}

BOOL menu() {
    system("cls");
    std::cout << "\033[6m\033[?25lF1 Enable X-ray\nF2 Disable X-ray\nF3 Open feature code file\n\033[32mDEL Uninstall program\033[0m" << std::endl;
    return TRUE;
};

// Refresh Offset every 200ms
DWORD WINAPI UpdataOffset(LPVOID lpParam) {
    int id = 0;
    id = *(int*)lpParam;
    if (id != 0) {
        std::cout << "Base address thread is running!" << id << std::endl;
    };
    do
    {
        char* code1 = GetDosContext();
        if (GetCs2XrayOffset(Tem_Offset, code1)) {
            Offset = Tem_Offset;
            XrayOpen = 0;
            delete[] code1;
            continue;
        }
        char* code2 = new char[strlen(code1) + 1];
        errno_t err = strncpy_s(code2, strlen(code1) + 1, code1, strlen(code1));

        code2[0] = '9';
        code2[1] = '0';
        code2[2] = '9';
        code2[3] = '0';

        if (GetCs2XrayOffset(Tem_Offset, code2)) {
            Offset = Tem_Offset;
            XrayOpen = 1;
            delete[] code1, code2;
            continue;
        }
        Offset = 0;
        XrayOpen = 0;
        delete[] code1, code2;
        continue;

        PauseMs(200);
    } while (id);
    return 0;
};

int main() {

    std::cout << "Please be patient! Base address updating..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    // Try to update base address
    int* id2 = new int(1);
    CreateThread(NULL, 0, UpdataOffset, id2, 0, 0);
    // Try first base address update
    int* id1 = new int(0);
    UpdataOffset(id1);
    // Wait for base address update
    if (Offset == 0) {
        std::cout << "Base address not found! Contact the author..." << std::endl << "Error Code: " << GetLastError() << std::endl;
    }
    else {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Base address found! Time taken: " << duration.count() << " ms" << std::endl << "Waiting to enter the helper..." << std::endl;
        PauseMs(2000);//PauseS_visual(2);
    }

    menu();

    while (TRUE) {
        if ((GetAsyncKeyState(0x70) & 0x8000)) { // F1
            if (Offset == 0) {
                InActivationSonic();
                continue;
            }
            if (wMem(L"cs2.exe", L"client.dll", Offset, 0x9090, 2)) {
                XrayOpen = TRUE;
                ActivationSonic();
            }
            else {
                std::cout << "Error Code: " << GetLastError() << std::endl;
            }
        }

        if ((GetAsyncKeyState(0x71) & 0x8000)) { // F2
            if (Offset == 0) {
                InActivationSonic();
                continue;
            }
            if (wMem(L"cs2.exe", L"client.dll", Offset, 0xC032, 2)) {
                XrayOpen = FALSE;
                InActivationSonic();
            }
            else {
                std::cout << "Error Code: " << GetLastError() << std::endl;
            }
        }

        if ((GetAsyncKeyState(0x72) & 0x8000)) { // F3
            char* userProfile = nullptr;
            size_t len = 0;
            errno_t err = _dupenv_s(&userProfile, &len, "USERPROFILE");
            if (err != 0 || userProfile == nullptr) {
                MessageBoxW(0, L"Unable to get the user profile path!", L"Error", 0);
                return -1;
            }
            std::string filePath = std::string(userProfile) + "\\Documents\\cs2Xray.ini";
            ReadfileInTXT(filePath);
        };

        if (GetAsyncKeyState(VK_DELETE) & 0x8000) { // DEL
            if (Offset != 0) {
                wMem(L"cs2.exe", L"client.dll", Offset, 0xC032, 2);
                XrayOpen = FALSE;
                InActivationSonic();
            }
            KillTheProsess();
        };

        PauseMs(10);
    }

    return 0;
}

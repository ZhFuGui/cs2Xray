#pragma once
#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>

inline void PauseS_visual(int seconds) noexcept {
    std::cout << seconds << " seconds until continuation..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    for (int i = seconds; i >= 0; --i) {
        system("cls");
        std::cout << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    system("cls");
}

// Close the current process
void KillTheProsess() noexcept {
    HANDLE CurrentProcess = GetCurrentProcess();
    if (CurrentProcess != NULL) {
        if (!TerminateProcess(CurrentProcess, 0)) {
            DWORD exitCode = GetLastError();
            CloseHandle(CurrentProcess);
        }
        else {
            CloseHandle(CurrentProcess);
        }
    }
};

// Time pause in milliseconds
void PauseMs(int milliseconds) noexcept {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// Activate sound
void ActivationSonic() {
    Beep(1700, 600);
}

// Deactivate sound
void InActivationSonic() {
    Beep(300, 700);
}

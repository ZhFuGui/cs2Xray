#pragma once
#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

BOOL ByteToChar(BYTE* byteCode, char* strCode, int codeLen)
{
    for (int i = 0; i < codeLen; i++) {
        wsprintfA(&strCode[i * 2], "%02X", byteCode[i]);
    }
    return TRUE;
}

BOOL cmpStrCode(char* code, char* readStr, int cmpLen)
{
    for (int i = 0; i < cmpLen; i++) {
        if (code[i] == '?') {
            continue;
        }
        if (code[i] != readStr[i]) {
            return FALSE;
        }
    }
    return TRUE;
};

BOOL scanGameCode(HANDLE hProcess, uint64_t beginAddr, uint64_t endAddr, char* code, int codeLen, DWORD64& retAddr) {

    BYTE* readCode = new BYTE[0x1000];

    for (uint64_t readAddr = beginAddr; readAddr <= endAddr - 0x1000; readAddr += 0x1000 - codeLen)
    {
        memset(readCode, 0, 0x1000);
        char strCode[0x2001] = { 0 };
        BOOL retRead = ReadProcessMemory(hProcess, (LPVOID)readAddr, readCode, 0x1000, NULL);
        if (retRead == 0)
        {
            continue;
        }
        // Convert byte array to string
        ByteToChar(readCode, strCode, 0x1000);
        // Compare strings to check if the signature matches
        for (int i = 0; i < 0x2001 - codeLen; i++) {
            BOOL retCmp = cmpStrCode(code, &strCode[i], codeLen);
            if (retCmp == TRUE)
            {
                retAddr = readAddr + i / 2;
                return TRUE;
            }
        }
    }
    return FALSE;
}

#pragma once
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <sstream>

// Open file operation in TXT format
void ReadfileInTXT(const std::string& str) {
    ShellExecuteA(NULL, "open", str.c_str(), NULL, NULL, SW_SHOW);
}

// Read the signature from the ini file
char* GetDosContext() {
    char* userProfile = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&userProfile, &len, "USERPROFILE");

    if (err != 0 || userProfile == nullptr) {
        MessageBoxW(0, L"Unable to get the user profile path!", L"Error", 0);
        return 0;
    }

    std::string filePath = std::string(userProfile) + "\\Documents\\cs2Xray.ini";

    std::ifstream inFile(filePath);
    std::string fileContent;

    if (inFile.is_open()) {

        std::getline(inFile, fileContent);
        inFile.close();

        if (fileContent.empty()) {
            MessageBoxW(0, L"The file is empty.", L"Error", 0);
            ReadfileInTXT(filePath);
            return 0;
        }

        // Remove all spaces
        fileContent.erase(std::remove(fileContent.begin(), fileContent.end(), ' '), fileContent.end());

        // Calculate the string length
        size_t length = fileContent.size();
        if (length == 0) {
            MessageBoxW(0, L"The file is empty.", L"Error", 0);
            return 0;
        }

        // Dynamically allocate memory
        char* str = new char[length + 1];  // +1 for the null-terminator '\0'
        if (!str) {
            MessageBoxW(0, L"Error parsing the string.", L"Error", 0);
            return 0;
        }

        // Copy content into the char array
        std::copy(fileContent.begin(), fileContent.end(), str);
        str[length] = '\0';  // Ensure the string ends with '\0'

        return str;
    }
    else {

        std::ofstream outFile(filePath);
        if (outFile.is_open()) {
            outFile.close();
        }
        MessageBoxW(0, L"File does not exist. A new cs2Xray.ini has been created. Please enter the signature string and run again.", L"Error", 0);
        ReadfileInTXT(filePath);
        return 0;
    }
}

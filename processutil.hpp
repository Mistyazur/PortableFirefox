#ifndef __PROCESS_HPP__
#define __PROCESS_HPP__

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <windows.h>


std::string ExecCmd(const char* cmd)
{
    std::string result;
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    return result;
}

std::string ExecCmd(const char *cmd, int *exitcode)
{
    std::string result;
    std::array<char, 128> buffer;
    auto deleter = [exitcode](FILE *f) {
        *exitcode = _pclose(f);
    };
    std::unique_ptr<FILE, decltype(deleter)> pipe(_popen(cmd, "r"), deleter);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    return result;
}

// 
// Pre-C++11 version:
// 

/*
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

std::string ExecCmd(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        _pclose(pipe);
        throw;
    }
    std::cout << "exit code : " << _pclose(pipe) << std::endl;
    return result;
}
*/

BOOL StartProcess(LPCWSTR lpCmd, LPCWSTR lpWorkingDir, DWORD *exitcode = nullptr)
{
    STARTUPINFO si = {};
    PROCESS_INFORMATION pi = {};
    WCHAR szCmd[MAX_PATH] = {0};

    wcscpy_s(szCmd, lpCmd);

    si.cb = sizeof(si);

    // Start the program up
    if (!CreateProcessW(NULL, // the path
                       szCmd,             // Command line
                       NULL,              // Process handle not inheritable
                       NULL,              // Thread handle not inheritable
                       FALSE,             // Set handle inheritance to FALSE
                       CREATE_NO_WINDOW,  // Creation flags
                       NULL,              // Use parent's environment block
                       lpWorkingDir,      // Use parent's starting directory
                       &si,               // Pointer to STARTUPINFO structure
                       &pi                // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
                       ))
        return FALSE;

    // Get exit code
    if (exitcode) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, exitcode);
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return TRUE;
}

BOOL StartProcessWithOutput(LPCWSTR lpCmd, LPCWSTR lpWorkingDir, std::string &output, DWORD *exitcode = nullptr)
{
    HANDLE hRead, hWrite;
    STARTUPINFO si = {};
    PROCESS_INFORMATION pi = {};
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // Inheritable handles
    WCHAR szCmd[MAX_PATH] = {0};
    CHAR buffer[4096] = {};
    DWORD bytesRead;

    wcscpy_s(szCmd, lpCmd);

    CreatePipe(&hRead, &hWrite, &saAttr, 0);

    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdOutput = hWrite;

    // Start the program up
    if (!CreateProcessW(NULL, // the path
                       szCmd,             // Command line
                       NULL,              // Process handle not inheritable
                       NULL,              // Thread handle not inheritable
                       TRUE,              // Set handle inheritance to FALSE
                       CREATE_NO_WINDOW,  // Creation flags
                       NULL,              // Use parent's environment block
                       lpWorkingDir,      // Use parent's starting directory
                       &si,               // Pointer to STARTUPINFO structure
                       &pi                // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
                       ))
        return FALSE;
    
    // Close the write end of the pipe (not needed by parent)
    CloseHandle(hWrite);

    // Read the output from the child process
    output.clear();
    while (ReadFile(hRead, buffer, sizeof(buffer), &bytesRead, NULL))
        output.append(buffer);
    CloseHandle(hRead);

    // Get exit code
    if (exitcode) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, exitcode);
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return TRUE;
}

#endif
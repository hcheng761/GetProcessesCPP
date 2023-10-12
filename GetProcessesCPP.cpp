// GetProcessesCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <Windows.h>
#include <string>
#include <stdio.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>

static int numProcessors;
static std::unordered_map<std::string,int>processSet;
static std::unordered_set<std::string>openProcessWindows;

std::string ProcessIDName(HANDLE handle, DWORD pid)
{
    std::string name;
    DWORD buffSize = 1024;
    CHAR buffer[1024];
    LPWSTR windowName;
    if (QueryFullProcessImageNameA(handle, 0, buffer, &buffSize))
    {
        name = buffer;
        processSet[name]++;
    }

    return name;
}

void ListProcessModules(DWORD dwPID)
{
    std::cout << "LIST OF PROCESS MODULES FOR " << dwPID << '\n';

    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    me32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hModuleSnap, &me32))
    {
        while (Module32Next(hModuleSnap, &me32))
        {
            std::cout << me32.th32ModuleID << '\n';
        }
    }

    CloseHandle(hModuleSnap);
    return;
}

BOOL CALLBACK enumWindowsCB(HWND hwnd, LPARAM lParam)
{

    DWORD buffSize = 1024;
    CHAR bufferd[1024];
    if (QueryFullProcessImageNameA(hwnd, 0, bufferd, &buffSize))
    {
        std::string s = bufferd;
        std::cout << s << '\n';
    }

    int length = GetWindowTextLength(hwnd);
    char* buffer = new char[length + 1];
    GetWindowText(hwnd, buffer, length + 1);
    std::string windowTitle(buffer);
    delete[] buffer;

    // List visible windows with a non-empty title
    std::unordered_set <std::string>& windows = *reinterpret_cast<std::unordered_set<std::string>*>(lParam);
    if (IsWindowVisible(hwnd) && length != 0) {
        std::cout  << windowTitle << std::endl;
        windows.insert(windowTitle);
    }
    return TRUE;
}

int main()
{
    SYSTEM_INFO sysInfo;
    HANDLE hProcsSnap;
    HANDLE sysModulesScreenshot;
    DWORD dwPriorityClass;

    system("color 07");
    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;
    
    hProcsSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    UINT32 critProcNum = 0;

    if (hProcsSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 prEntry;
        prEntry.dwSize = sizeof(PROCESSENTRY32);
        
        if (!Process32First(hProcsSnap, &prEntry))
        {
            std::cout << "PROCESS32FIRST" << '\n';
            CloseHandle(hProcsSnap);
            return 0;
        }

        HANDLE hProcess;
        while (Process32Next(hProcsSnap, &prEntry))
        {
            dwPriorityClass = 0;
            hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, prEntry.th32ProcessID);
            BOOL critProc;

            if (hProcess && IsProcessCritical(hProcess, &critProc))
            {
                ProcessIDName(hProcess, prEntry.th32ProcessID);
                critProcNum++;
            }
        }
        //Current open windows
        EnumWindows(enumWindowsCB, reinterpret_cast<LPARAM>(&openProcessWindows));
        CloseHandle(hProcsSnap);
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

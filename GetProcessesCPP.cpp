// GetProcessesCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <Windows.h>
#include <string>
#include <stdio.h>
#include <TlHelp32.h>

static int numProcessors;


int main()
{
    SYSTEM_INFO sysInfo;
    HANDLE hProcsSnap;
    HANDLE sysModulesScreenshot;
    DWORD dwPriorityClass;

    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;
    
    hProcsSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

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

        do {
            dwPriorityClass = 0;
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, prEntry.th32ProcessID);

            if (hProcess != NULL)
            {
                dwPriorityClass = GetPriorityClass(hProcess);
                CloseHandle(hProcess);
                std::cout << prEntry.th32ProcessID << '\n';
            }

        } while (Process32Next(hProcsSnap, &prEntry));
        CloseHandle(hProcsSnap);
    }

    //HWND hwnd = FindWindowA(0, "Calculator");
    //GetWindowThreadProcessId(hwnd, &ProcessID);
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

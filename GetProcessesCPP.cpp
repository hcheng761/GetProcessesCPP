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
#include <map>

static int numProcessors;
static std::map<std::wstring,std::vector<DWORD>>processMap; //executable and # of subprocesses

bool CheckIfChildProcess(HANDLE p, HANDLE c)
{
    return false;
}

std::wstring ProcessIDName(HANDLE handle, DWORD pid)
{
    std::wstring name;
    DWORD buffSize = MAX_PATH;
    wchar_t* path = new wchar_t[MAX_PATH];
    LPWSTR windowName;

    if (QueryFullProcessImageNameW(handle, 0, path, &buffSize))
    {
        name = path;
        if (processMap.find(name) != processMap.end())
            processMap[name].push_back(pid);

        std::wcout << name << ": " << pid << '\n';
    }

    return name;
}

BOOL CALLBACK enumWindowsCB(HWND hwnd, LPARAM lParam)
{
    if (!IsWindowVisible(hwnd))
        return TRUE;

    int length = GetWindowTextLength(hwnd) + 1;
    wchar_t* buffer = new wchar_t[length];
    GetWindowTextW(hwnd, buffer, length);
    DWORD id; GetWindowThreadProcessId(hwnd, &id);

    wchar_t* path = new wchar_t[MAX_PATH];
    DWORD size = MAX_PATH;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, id);

    if (QueryFullProcessImageNameW(hProc, 0, path, &size))
    {
        std::wstring ws = path;
        if (processMap.find(ws) == processMap.end())
            processMap[ws] = std::vector<DWORD>();
        std::wcout << ws << ": " << id << '\n';
    }
    
    //std::unordered_set <std::string>& windows = *reinterpret_cast<std::unordered_set<std::string>*>(lParam);
    return TRUE;
}

void GetCPUUsage(DWORD pid)
{
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        std::wcout << pmc.QuotaPeakPagedPoolUsage << '\n';
        std::wcout << pmc.QuotaPeakNonPagedPoolUsage << '\n';
    }

    CloseHandle(hProcess);
}

int main()
{
    SYSTEM_INFO sysInfo;
    HANDLE hProcsSnap;
    HANDLE sysModulesScreenshot;

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
            CloseHandle(hProcsSnap);
            return 0;
        }

        EnumWindows(enumWindowsCB, reinterpret_cast<LPARAM>(&processMap));
        std::cout << '\n' << '\n';
        HANDLE hProcess;
        while (Process32Next(hProcsSnap, &prEntry))
        {
            hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, prEntry.th32ParentProcessID);
            BOOL critProc;

            if (hProcess && IsProcessCritical(hProcess, &critProc))
            {
                ProcessIDName(hProcess, prEntry.th32ParentProcessID);
                critProcNum++;
            }
        }

        for (auto& [key, v] : processMap)
        {
            for (auto& vi : v)
                std::wcout << key << ": " << vi << '\n';
            std::cout << '\n';
        }

        CloseHandle(hProcsSnap);

        DWORD cpid;
        HWND chromeHwnd = FindWindowA(0, ("Spotify"));
        GetWindowThreadProcessId(chromeHwnd, &cpid);

        return 0;
        while (true)
        {
            GetCPUUsage(5776);
            Sleep(1000);
        }
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

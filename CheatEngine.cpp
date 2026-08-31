#include "CheatEngine.h"
#include "Functions.h"
#include "CMsgHandler.h"
#include <windows.h>
#include <psapi.h>
#include <wtsapi32.h>
#include <unordered_map>
#include <array>
#include <string>
#include <fstream>

#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Psapi.lib")

typedef BOOL(WINAPI* lpQueryFullProcessImageNameA)(HANDLE, DWORD, LPSTR, DWORD*);
lpQueryFullProcessImageNameA MyQueryFullProcessImageNameA = (lpQueryFullProcessImageNameA)
GetProcAddress(GetModuleHandleA("kernel32.dll"), "QueryFullProcessImageNameA");

std::string CheatEngine::RemoveName(const std::string& FilePath) {
    size_t LastSlash = FilePath.find_last_of("\\/");
    return (LastSlash == std::string::npos) ? FilePath : FilePath.substr(0, LastSlash);
}

BOOL CheatEngine::CheckExist(const std::string& FilePath) {
    std::ifstream File(FilePath);
    return File.is_open();
}

std::string CheatEngine::GetFullName(DWORD ProcessId) {
    std::string FullName = GetFullNameQuery(ProcessId);
    if (!FullName.empty()) return FullName;

    FullName = GetFullNameModule(ProcessId, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);
    if (!FullName.empty()) return FullName;

    FullName = GetFullNameModule(ProcessId, PROCESS_QUERY_LIMITED_INFORMATION);
    if (!FullName.empty()) return FullName;

    return GetFullNameModule(ProcessId, PROCESS_QUERY_INFORMATION);
}

std::string CheatEngine::GetFullNameQuery(DWORD ProcessId) {
    CHAR Path[MAX_PATH] = { 0 };
    DWORD Size = MAX_PATH;

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ProcessId);
    if (hProc) {
        if (MyQueryFullProcessImageNameA && MyQueryFullProcessImageNameA(hProc, 0, Path, &Size)) {
            CloseHandle(hProc);
            return Path;
        }
        CloseHandle(hProc);
    }
    return "";
}

std::string CheatEngine::GetFullNameModule(DWORD ProcessId, DWORD Access) {
    CHAR Path[MAX_PATH] = { 0 };
    HANDLE hProc = OpenProcess(Access, FALSE, ProcessId);

    if (hProc) {
        if (GetModuleFileNameExA(hProc, 0, Path, MAX_PATH)) {
            CloseHandle(hProc);
            return Path;
        }
        CloseHandle(hProc);
    }
    return "";
}

std::string WideToString(LPWSTR wideStr) {
    if (!wideStr) return "";
    int sizeNeeded = WideCharToMultiByte(CP_ACP, 0, wideStr, -1, NULL, 0, NULL, NULL);
    if (sizeNeeded <= 0) return "";
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_ACP, 0, wideStr, -1, &result[0], sizeNeeded, NULL, NULL);
    return result;
}

VOID CheatEngine::CheckFiles(const std::string& FullPath) {
    static const std::array<const char*, 20> SuspiciousFiles = {
        "lua53-32.dll", "lua53-64.dll", "luaco-32.dll", "am818.dll",
        "ollydbg.exe", "x64dbg.exe", "x32dbg.exe", "ida.exe", "ida64.exe",
        "processhacker.exe", "scylla.exe", "scylla_x64.exe", "scylla_x86.exe",
        "ollydbg.dll", "x64dbg.dll", "ida.dll", "ida64.dll",
        "processhacker.dll", "scylla.dll", "scylla_x86.dll"
    };

    std::string Folder = RemoveName(FullPath);
    for (const auto& file : SuspiciousFiles) {
        std::string full = Folder + "\\" + file;
        if (CheckExist(full)) {
            if (OwnerState::InGame) {
                OwnerState::Detected(Cheatengine, const_cast<char*>(file));
            }
            else {
                std::string reason = "Detection: " + std::string(file);
                API::CFunctions::ExitDetection(reason.c_str());
            }
            break;
        }
    }
}


DWORD  WINAPI  CheatEngine::OnProcess(LPVOID lpParam) {
    MessageBoxA(nullptr, "Detected process", "Cheat Detection", MB_OK | MB_ICONWARNING);

    WTS_PROCESS_INFO* pInfo = nullptr;
    DWORD Count = 0;
    if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pInfo, &Count)) {
        CHAR systemDir[MAX_PATH] = { 0 };
        GetSystemDirectoryA(systemDir, MAX_PATH);
        std::string systemDirStr = systemDir;
        std::string windowsRoot = systemDirStr.substr(0, systemDirStr.find("\\System32"));

        const std::unordered_map<std::string, std::string> SystemProcessPaths = {
            {"svchost.exe", systemDirStr + "\\svchost.exe"},
            {"winlogon.exe", systemDirStr + "\\winlogon.exe"},
            {"csrss.exe", systemDirStr + "\\csrss.exe"},
            {"services.exe", systemDirStr + "\\services.exe"},
            {"explorer.exe", windowsRoot + "\\explorer.exe"},
        };

        for (DWORD i = 0; i < Count; ++i) {
            if (!pInfo[i].pProcessName) continue;

            std::string procName = WideToString(pInfo[i].pProcessName);
            std::string fullPath = GetFullName(pInfo[i].ProcessId);

            auto it = SystemProcessPaths.find(procName);
            if (it != SystemProcessPaths.end()) {
                const std::string& legitPath = it->second;

                if (_stricmp(fullPath.c_str(), legitPath.c_str()) != 0) {
                    if (OwnerState::InGame) {
                        OwnerState::Detected(Cheatengine, "Dbg");
                    }
                    else {
                        API::CFunctions::ExitDetection("Detection Dbg");
                    }
                    continue;
                }
            }

            if (!fullPath.empty()) {
                CheckFiles(fullPath);
            }
        }
        WTSFreeMemory(pInfo);
    }
    return 1;
}

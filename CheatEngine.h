#pragma once
#include "Common.h"
#include <string>
#include <fstream>

class CheatEngine {
public:
    static DWORD WINAPI CheatEngine::OnProcess(LPVOID lpParam);
    static VOID CheckFiles(const std::string& path);
    static std::string GetFullName(DWORD pid);
    static BOOL CheckExist(const std::string& path);
    static std::string RemoveName(const std::string& path);
private:
    static std::string GetFullNameQuery(DWORD pid);
    static std::string GetFullNameModule(DWORD pid, DWORD access);
};

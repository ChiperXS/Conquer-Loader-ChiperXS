#pragma once
#include "Common.h"
#include <Aclapi.h>
#include <ShlObj.h>
#include "Constants.h"
#include <mutex>
#pragma warning(push)
#pragma warning(disable: 4091)

class RegKey {
public:
    static std::string ProSerial;
    static bool InitializeMutex();
    static void EncryptDecrypt(std::string& Data);
    static void SetRegValue();
    static void Read();
};
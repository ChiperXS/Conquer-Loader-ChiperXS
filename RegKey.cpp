#include "RegKey.h"
#include <windows.h>
#include <AclAPI.h>
#include <fstream>
#include <string>
#include <iostream>

string RegKey::ProSerial;

HANDLE hMutex = NULL;

bool RegKey::InitializeMutex() {
    hMutex = CreateMutexA(NULL, FALSE, "Global\\SilentGameMutex");

    if (hMutex == NULL) {
        MessageBoxA(0, "Failed to create mutex.", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

void RegKey::EncryptDecrypt(std::string& Data) {
    unsigned char key = 0x58;
    for (size_t i = 0; i < Data.size(); ++i) {
        Data[i] ^= key;
    }
}

void RegKey::SetRegValue() {
    if (hMutex == NULL)
        return;

    DWORD waitResult = WaitForSingleObject(hMutex, INFINITE);
    if (waitResult != WAIT_OBJECT_0)
        return;

    std::string folderName = API::Constants::VaultFileName;
    std::string folderPath = "C:\\ProgramData\\" + folderName;

    if (!CreateDirectoryA(folderPath.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ReleaseMutex(hMutex);
        return;
    }

    SetFileAttributesA(folderPath.c_str(), FILE_ATTRIBUTE_HIDDEN);

    std::string filePath = folderPath + "\\Serial.txt";
    ProSerial = API::CFunctions::GetRandomString(12);
    EncryptDecrypt(ProSerial);

    std::ofstream file(filePath, std::ios::binary);
    if (file.is_open()) {
        file << ProSerial;
        file.close();

        EXPLICIT_ACCESSA ea = {};
        PACL pACL = NULL;
        PSECURITY_DESCRIPTOR pSD = NULL;

        ea.grfAccessPermissions = GENERIC_READ;
        ea.grfAccessMode = SET_ACCESS;
        ea.grfInheritance = NO_INHERITANCE;
        ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
        ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
        ea.Trustee.ptstrName = (LPSTR)"SYSTEM";

        if (SetEntriesInAclA(1, &ea, NULL, &pACL) == ERROR_SUCCESS) {
            pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
            if (pSD && InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) {
                if (SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE)) {
                    SetFileSecurityA(filePath.c_str(), DACL_SECURITY_INFORMATION, pSD);
                }
            }
        }

        if (pSD) LocalFree(pSD);
        if (pACL) LocalFree(pACL);
    }

    ReleaseMutex(hMutex);
}

void RegKey::Read() {
    if (hMutex == NULL)
        return;

    DWORD waitResult = WaitForSingleObject(hMutex, INFINITE);
    if (waitResult != WAIT_OBJECT_0)
        return;

    std::string folderName = API::Constants::VaultFileName;
    std::string filePath = "C:\\ProgramData\\" + folderName + "\\Serial.txt";

    for (int i = 0; i < 5; ++i) {
        std::ifstream file(filePath, std::ios::binary);
        if (file.is_open()) {
            std::getline(file, ProSerial);
            file.close();

            if (!ProSerial.empty()) {
                EncryptDecrypt(ProSerial);
                break;
            }
        }
        Sleep(120);
    }

    if (ProSerial.empty()) {
        SetRegValue();
    }

    ReleaseMutex(hMutex);
}

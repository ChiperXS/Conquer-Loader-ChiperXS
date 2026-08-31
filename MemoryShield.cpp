#include "MemoryShield.h"
#include "Memory.h"
#include "ZFHash.h"
#include "OwnerState.h"
#include <cstring>
#include <thread>

namespace CShield {

    const SIZE_T MemoryShield::MemSize5517 = 4466225;
    LPVOID MemoryShield::AddrStart5517 = reinterpret_cast<LPVOID>(0x00401008);
    BYTE* MemoryShield::FirstTqDump5517 = new BYTE[MemSize5517];
    BYTE* MemoryShield::SecondTqDump5517 = new BYTE[MemSize5517];
    BYTE* MemoryShield::InitialGameDump5517 = new BYTE[MemSize5517];
    BYTE* MemoryShield::InitialGameDumpEncrypted5517 = new BYTE[MemSize5517];
    bool MemoryShield::bInitialCheckDone = false;
    bool MemoryShield::bGameStarted = false;
    char MemoryShield::InitialHash[64] = { 0 };

    std::mutex MemoryShield::ProcessMutex;
    std::atomic<bool> MemoryShield::IsProcessing = false;

    DWORD  WINAPI MemoryShield::InitialGameCheck(LPVOID lpParam) {
        Sleep(5000);
        memset(InitialGameDump5517, 0, MemSize5517);
        memset(InitialGameDumpEncrypted5517, 0, MemSize5517);
        SIZE_T BytesRead = 0;

        API::CMemory::Read(AddrStart5517, InitialGameDump5517, MemSize5517, &BytesRead);

        if (BytesRead == MemSize5517) {
            memcpy(InitialGameDumpEncrypted5517, InitialGameDump5517, MemSize5517);
            MemChiper1(InitialGameDumpEncrypted5517, MemSize5517);

            API::ZFHash hasher;
            char* hash = hasher.DigestMemory(InitialGameDumpEncrypted5517, MemSize5517);
            if (hash) {
                strncpy(InitialHash, hash, sizeof(InitialHash) - 1);
                InitialHash[sizeof(InitialHash) - 1] = '\0';
            }

            bInitialCheckDone = true;
        }
        return 0;
    }

    BOOL MemoryShield::CheckMemoryIntegrity() {
        if (!bInitialCheckDone)
            return TRUE;

        BYTE* CurrentDump = new BYTE[MemSize5517];
        memset(CurrentDump, 0, MemSize5517);
        SIZE_T BytesRead = 0;

        API::CMemory::Read(AddrStart5517, CurrentDump, MemSize5517, &BytesRead);

        if (BytesRead != MemSize5517) {
            delete[] CurrentDump;
            return TRUE;
        }

        MemChiper1(CurrentDump, MemSize5517);

        API::ZFHash hasher;
        char* CurrentHash = hasher.DigestMemory(CurrentDump, MemSize5517);

        bool bMemoryChanged = false;
        if (CurrentHash && InitialHash[0] != '\0') {
            bMemoryChanged = (strcmp(InitialHash, CurrentHash) != 0);
        }

        delete[] CurrentDump;

        if (bMemoryChanged) {
            TerminateGame();
            return FALSE;
        }

        return TRUE;
    }

    BOOL MemoryShield::QuickMemoryCheck() {
        if (!bInitialCheckDone)
            return TRUE;

        BYTE* CurrentDump = new BYTE[MemSize5517];
        memset(CurrentDump, 0, MemSize5517);
        SIZE_T BytesRead = 0;

        API::CMemory::Read(AddrStart5517, CurrentDump, MemSize5517, &BytesRead);

        if (BytesRead != MemSize5517) {
            delete[] CurrentDump;
            return TRUE;
        }

        bool bMemoryChanged = (memcmp(InitialGameDump5517, CurrentDump, MemSize5517) != 0);

        delete[] CurrentDump;

        if (bMemoryChanged) {
            TerminateGame();
            return FALSE;
        }

        return TRUE;
    }

    void MemoryShield::GetFirstDump() {
        memset(FirstTqDump5517, 0, MemSize5517);
        API::CMemory::Read(AddrStart5517, FirstTqDump5517, MemSize5517, NULL);
        MemChiper(FirstTqDump5517, MemSize5517);
        bGameStarted = true;
    }

    void MemoryShield::OnProcess() {
        if (!bGameStarted || IsProcessing.exchange(true)) 
            return;
        std::thread([]() {
            std::lock_guard<std::mutex> lock(ProcessMutex);
            CMsgMemory CPacket;
            SIZE_T NumberOfBytes = 0;
            memset(SecondTqDump5517, 0, MemSize5517);
            API::CMemory::Read(AddrStart5517, SecondTqDump5517, MemSize5517, &NumberOfBytes);

            if (NumberOfBytes == MemSize5517) {
                MemChiper(SecondTqDump5517, MemSize5517);
                API::ZFHash hasher;
                sprintf(CPacket.FirstMemoryDump, "%s", hasher.DigestMemory(FirstTqDump5517, MemSize5517));
                sprintf(CPacket.SecondMemoryDump, "%s", hasher.DigestMemory(SecondTqDump5517, MemSize5517));
                OwnerState::SendMsg(&CPacket);
            }
            IsProcessing = false;
            }).detach();
    }

    void MemoryShield::TerminateGame() {
        MessageBoxA(NULL, "Memory Modification Detected!.", "Anti-Cheat", MB_OK | MB_ICONERROR);
        ExitProcess(0);
    }
}

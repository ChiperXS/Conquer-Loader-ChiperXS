#pragma once
#include "Common.H"
namespace API {
    class Constants {
    public:
        static uint16_t AuthPort;
        static uint16_t GamePort;
        static FARPROC Adress_SpeedCheat;     // Address of GetTickCount
        static FARPROC Adress_SpeedOther;     // Address of timeGetTime
        static unsigned char pBytesSpeedCheat[8];
        static unsigned char pBytesSpeedOther[8];
        static const std::string GameIP;
        static const std::string NewMutex;
        static const std::string LoginKey;
        static const std::string DisocrdAppID;
        static const std::string VaultFileName;
        static const std::string ServerName;
        static const BOOL TestStatus;
        static int FpsValue;
        static int DiscordStatus;
        static int16_t GameVersion;
        static const int32_t keys[16];

        static BOOL IsGamePort(uint16_t InPort);
        static BOOL IsValidPort(uint16_t InPort);
    };
}

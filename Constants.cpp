#include "Constants.h"

namespace API {
    uint16_t Constants::AuthPort = 9958;
    uint16_t Constants::GamePort = 5816;
    FARPROC Constants::Adress_SpeedCheat = nullptr;
    FARPROC Constants::Adress_SpeedOther = nullptr;
    unsigned char Constants::pBytesSpeedCheat[8] = { 0 };
    unsigned char Constants::pBytesSpeedOther[8] = { 0 };
    const std::string Constants::GameIP = "46.105.76.161";
    const std::string Constants::NewMutex = "Bla99ck1y";
    const std::string Constants::LoginKey = "A154mr20cod0IN2G";
    const std::string Constants::DisocrdAppID = "";
    const std::string Constants::VaultFileName = "SilentVault";
    const std::string Constants::ServerName = "BlackBulls-Protection";
    const BOOL Constants::TestStatus = false;
    int Constants::FpsValue;
    int Constants::DiscordStatus;
    int16_t Constants::GameVersion = 1000;

    const int32_t Constants::keys[16] = {
        0x3F2A9D1C, 0x5C7E8A4B, 0x2B1F6D39, 0x7A4C3E91,
        0x6D3B1F72, 0x4E2C8A5F, 0x1B9D3C7A, 0x3A7F9E2D,
        0x5E1C6B3A, 0x6F2D7C1B, 0x7B4A2E5C, 0x2D8C3F91,
        0x4F9E1B7A, 0x3C7D2A6E, 0x1A5B4E93, 0x6E3F7D2C
    };

    BOOL Constants::IsGamePort(uint16_t InPort) {
        return (InPort >= GamePort && InPort <= GamePort + 4);
    }

    BOOL Constants::IsValidPort(uint16_t InPort) {
        if (InPort >= AuthPort && InPort <= AuthPort + 4)
            return true;
        if (InPort == 9800)
            return true;
        return IsGamePort(InPort);
    }
}

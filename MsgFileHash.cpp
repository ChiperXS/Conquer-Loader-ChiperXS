#include "MsgFileHash.h"
namespace Msg {
    VOID MsgFileHash::ClientFilesScanning() {
        CHAR Hash_Conquer[33] = { 0 };
        CHAR Hash_DLL[33] = { 0 };
        CHAR Hash_MT[33] = { 0 };
        CHAR Hash_ME[33] = { 0 };
        CHAR Hash_SR[33] = { 0 };

        API::CFunctions::MD5HASH("Conquer.exe", Hash_Conquer);
        API::CFunctions::MD5HASH("SilentVault.dll", Hash_DLL);
        API::CFunctions::MD5HASH("ini/MagicType.dat", Hash_MT);
        API::CFunctions::MD5HASH("ini/MagicEffect.ini", Hash_ME);
        API::CFunctions::MD5HASH("ini/StrRes.ini", Hash_SR);
        CMsgFileHash* packet = new CMsgFileHash();

        memset(packet->CoFile, 0, 33);
        memset(packet->DlFile, 0, 33);
        memset(packet->MTFile, 0, 33);
        memset(packet->MEFile, 0, 33);
        memset(packet->SRFile, 0, 33);

        if (strlen(Hash_Conquer) == 32) {
            memcpy(packet->CoFile, Hash_Conquer, 32);
        }
        if (strlen(Hash_DLL) == 32) {
            memcpy(packet->DlFile, Hash_DLL, 32);
        }
        if (strlen(Hash_MT) == 32) {
            memcpy(packet->MTFile, Hash_MT, 32);
        }
        if (strlen(Hash_ME) == 32) {
            memcpy(packet->MEFile, Hash_ME, 32);
        }
        if (strlen(Hash_SR) == 32) {
            memcpy(packet->SRFile, Hash_SR, 32);
        }
        OwnerState::SendMsg((uint8_t*)packet);
        delete packet;
    }
}
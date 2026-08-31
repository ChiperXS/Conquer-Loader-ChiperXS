#include "Common.h"
#include "CipherPacket.h"

namespace Crypt {
    BYTE GameBufIV[32] = { 89, 80, 40, 35, 33, 90, 44, 15, 60, 91, 23, 46, 40, 54, 99, 33, 142, 4, 19, 113, 106, 59, 215, 210, 169, 232, 121, 79, 241, 95, 193, 149 };
    BYTE GameBufKey[32] = { 70, 28, 28, 59, 10, 18, 18, 13, 154, 52, 11, 31, 84, 99, 83, 190, 57, 26, 89, 218, 121, 217, 232, 36, 55, 224, 48, 169, 254, 163, 166, 43 };

    VOID CipherPacket::GameEncrypt(uint8_t* pBuf, int16_t Length) {
        int8_t index = 4;
        for (BYTE x = index; x < Length - index; x++) {
            *(BYTE*)(pBuf + x) = (BYTE)(*(BYTE*)(pBuf + x) ^ GameBufIV[x * 44 % 32]);
            *(BYTE*)(pBuf + x) = (BYTE)(*(BYTE*)(pBuf + x) ^ GameBufKey[x * 99 % 32]);
        }
    }

    VOID CipherPacket::GameDecrypt(uint8_t* pBuf, int16_t Length) {
        int8_t index = 4;
        for (BYTE x = index; x < Length - index; x++) {
            *(BYTE*)(pBuf + x) = (BYTE)(*(BYTE*)(pBuf + x) ^ GameBufKey[x * 99 % 32]);
            *(BYTE*)(pBuf + x) = (BYTE)(*(BYTE*)(pBuf + x) ^ GameBufIV[x * 44 % 32]);
        }
    }
}
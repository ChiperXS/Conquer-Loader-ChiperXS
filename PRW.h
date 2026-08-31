#pragma once
#include "Common.H"

namespace API {
	class CPRW
	{
	public:
		template <typename T>
		static inline T Read(LPVOID lpAddress) {
			return *(T*)lpAddress;
		}

		static CHAR* ReadStringLen(uint8_t* buf, int16_t index, int16_t Length) {
			if (Length <= 0) return nullptr;

			CHAR* Dest = (CHAR*)malloc(sizeof(CHAR) * (Length + 1));
			if (Dest == nullptr) return nullptr;

			Dest[Length] = '\0';
			memcpy(Dest, buf + index, Length);
			return Dest;
		}
		static std::string ReadStringLen1(uint8_t* buf, int16_t index, int16_t Length) {
			if (Length <= 0 || index < 0) return "";
			char* Dest = new char[Length + 1];
			memcpy(Dest, buf + index, Length);
			Dest[Length] = '\0';
			std::string result(Dest);
			delete[] Dest;
			return result;
		}


		template<typename T>
		static inline VOID Write(LPVOID lpAddress, T _tValue) {
			*(T*)lpAddress = _tValue;
		}

		static VOID WriteString(uint8_t* buf, CHAR * value, int16_t len, int8_t offset) {
			for (int16_t i = 0; i < len; i++) {
				buf[offset + i] = value[i];
			}
		}

		static VOID PasswordEncrypt(CHAR* pBuf, int8_t Length) {
			BYTE GameBufIV[32] = { 45,12,78,231,99,64,200,17,123,42,87,19,55,91,33,144,201,5,76,188,22,157,244,39,111,92,203,71,255,10,166,77 };
			BYTE GameBufKey[32] = { 88,29,47,63,120,7,199,21,142,53,91,28,77,102,45,187,62,39,94,210,131,207,225,41,59,231,58,179,245,173,177,52 };

			if (Length > 32) Length = 32;

			for (int8_t FL = 0; FL < Length; FL++) {
				pBuf[FL] ^= GameBufIV[(44 * FL) % 32];
				pBuf[FL] ^= GameBufKey[(99 * FL) % 32];
			}
		}
	};
}
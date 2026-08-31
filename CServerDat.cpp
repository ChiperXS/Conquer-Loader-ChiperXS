#include "CServerDat.h"
#include "Constants.h"
namespace CO {
	DWORD Encrypt = 0;
	DWORD pSubDump = 0;
	BOOL IsReturned = false;
	BYTE Skip[] = { JNZ_OPCODE };
	constexpr BYTE Skip5695[] = { JNZ_OPCODE };
	auto BASE_JMP_ENC = reinterpret_cast<LPVOID>(0x00765716 + 1);

	auto BASE_JMP_ENCa = reinterpret_cast<LPVOID>(0x0075AD82 + 1);

	BYTE BufKey[32] = { 93, 207, 18, 246, 65, 11, 158, 224,39, 190, 143, 76, 88, 132, 21, 199,104, 59, 173, 6, 250, 121, 37, 68,99, 185, 14, 240, 71, 160, 127, 38 };

	VOID CServerDat::DatDecrypt(CHAR* Out, int32_t Size) {
		int32_t length = sizeof BufKey;
		for (int32_t FL = 0; FL < Size; FL++) {
			Out[FL] ^= BufKey[(FL + 1) % length];
			Out[FL] ^= BufKey[FL % length];
		}
	}
	DWORD* WINAPIV CServerDat::LoadFileDetour(CHAR* FileName, CHAR* Mode, int32_t FileHandle) {
		if (!IsReturned) {
			FILE* source = fopen(".\\SilentVault1.dat", "rb");

			if (source) {

				fseek(source, 0, SEEK_END);
				long fileSize = ftell(source);
				fseek(source, 0, SEEK_SET);

				CHAR* encryptedBuffer = new CHAR[fileSize];
				fread(encryptedBuffer, 1, fileSize, source);
				fclose(source);

				//DatDecrypt(encryptedBuffer, fileSize);

				FILE* dest = fopen(FileName, "wb");
				if (dest) {
					fwrite(encryptedBuffer, 1, fileSize, dest);
					fclose(dest);
				}

				delete[] encryptedBuffer;
				IsReturned = true;

			}
		}

		return reinterpret_cast<DWORD * (WINAPIV*)(CHAR*, CHAR*, int32_t)>(pSubDump)(FileName, Mode, FileHandle);
	}

	VOID CServerDat::Hook() {
		API::CMemory::WriteSafe(BASE_JMP_ENC, Skip, sizeof Skip);
		pSubDump = API::CMemory::AOBScan("\x83\xEC\x5C\x8B\x44\x24\x64\x53\x55\x56\x8B\x74\x24\x6C\x57\x33\xDB\x83\xCF\xFF\x8D\x4C\x24\x1C", "xxxxxxxxxxxxxxxxxxxxxxxx");

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(LPVOID&)pSubDump, LoadFileDetour);
		if (DetourTransactionCommit() != NO_ERROR) {
			exit(0);
			return;
		}
	}
}
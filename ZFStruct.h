#pragma once
#include "Common.h"

enum CheatFlags {
	None = 0,
	Program = 2,
	AutoClicker = 4,
	AutoKeyboard = 6,
	SpeedCClient = 8,
	Debugger = 10,
	InjectDll = 12,
	CloseingThread = 14,
	ChangeConquerFile = 16,
	Cheatengine = 18
};

enum SubType {
	DoLogin = 10,
	ThreadCheck = 20,
	MemoryCheck = 30,
	HackDetected = 40,
	DiscordTitle1 = 50,
	DiscordTitle2 = 60,
	ProcessCheck = 70,
	ConquerFileHash = 80,
	DllCheak = 90,
	StartStopAction = 100,
	StartStopAction1 = 110,
	RestrictedMapsList = 120,
	StartStopActionResponse = 130,

};

enum PacketID {
	MsgShield = 10050,
	Authentication = 1086,
	MsgMemoryCheck = 10053,
	MsgCloseClient = 10054,
	MsgRequestLogin = 10055,
	CMsgPlayerCommands = 10057,
	CMsgDllRecv = 10058
};

enum ClientPackets {
	Attack = 1022,
	DataMap = 10010,
	Usage = 1009,
	Chat = 1004,
	Login = 1052,
	Forward = 1055,
	AccServerLogin = 1542,
};

typedef struct {
	int16_t Size;
	int16_t PacketID;
} PacketHead;

struct CMsgCheat {
	PacketHead Head;
	int16_t SubType;
	int16_t CheatFlags;
	int32_t lReaosnString;
	CHAR ReaosnString[32];
	CMsgCheat() {
		Head.Size = sizeof(CMsgCheat);
		Head.PacketID = MsgShield;
		SubType = HackDetected;
		CheatFlags = 0;
		lReaosnString = 0;
		memset(ReaosnString, 0, sizeof(ReaosnString));
	}
};

struct CMsgMemory {
	PacketHead Head;
	CHAR FirstMemoryDump[17];
	CHAR SecondMemoryDump[17];
	CMsgMemory() {
		Head.Size = sizeof(CMsgMemory);
		Head.PacketID = MsgMemoryCheck;
		memset(FirstMemoryDump, 0, sizeof(FirstMemoryDump));
		memset(SecondMemoryDump, 0, sizeof(SecondMemoryDump));
	}
};

enum ProcessesType {
	Start = 110,
	Insert = 220,
	Finish = 330,
};

struct CMsgOpenedProcesses {
	PacketHead Head;
	int16_t ActionType;
	int16_t ProgramLen;
	CHAR OneProgram[128];
	CMsgOpenedProcesses() {
		Head.Size = sizeof(CMsgOpenedProcesses);
		Head.PacketID = 10056;
		ActionType = 0;
		ProgramLen = 0;
		memset(OneProgram, 0, sizeof(OneProgram));
	}
};
struct CMsgFileHash {
	PacketHead Head;
	int16_t SubType;
	CHAR CoFile[33];
	CHAR DlFile[33];
	CHAR MTFile[33];
	CHAR MEFile[33];
	CHAR SRFile[33];
	CMsgFileHash() {
		Head.Size = sizeof(CMsgFileHash);
		Head.PacketID = MsgShield;
		SubType = ConquerFileHash;
	}
};

struct CMsgStartStop {
	PacketHead Head;
	int16_t SubType;
	int16_t ActionType;
	CMsgStartStop() {
		Head.Size = sizeof(CMsgStartStop);
		Head.PacketID = MsgShield;
		SubType = 100;
		ActionType = 0;
	}
};
struct CMsgRestrictedMaps {
	PacketHead Head;
	int16_t SubType;
	int16_t MapCount;
	int16_t MapIDs[50];

	CMsgRestrictedMaps() {
		Head.Size = sizeof(CMsgRestrictedMaps);
		Head.PacketID = MsgShield;
		SubType = RestrictedMapsList;
		MapCount = 0;
		memset(MapIDs, 0, sizeof(MapIDs));
	}
};
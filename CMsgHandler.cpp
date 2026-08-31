#include "PRW.h"
#include "Owner.h"
#include "RegKey.h"
#include "Discord.h"
#include "CTqPacket.h"
#include "CMsgHandler.h"
#include "DetourAddres.h"
#include "CHotKey.h"
#include "CSnprintf.h"
#include "Constants.h"
#include "ThreadBool.h"
#include "AutoHuntGUI.h"
#include <unordered_map>
#include <cstdlib>
#include <ctime>

namespace CMsg {

	VOID CMsgHandler::SendStartStopAction(int16_t actionType)
	{
		if (!OwnerState::InGame)
			return;

		CMsgStartStop* packet = new CMsgStartStop();
		packet->ActionType = actionType;
		OwnerState::SendMsg((VOID*)packet);
	}

	void CMsgHandler::Cleanup()
	{
		AutoHuntGUI::Cleanup();
	}

	uint32_t Encrypt(uint32_t value) {
		for (int i = 0; i < 8; ++i) {
			value = (value ^ API::Constants::keys[i]) + API::Constants::keys[7 - i];
			value = (value << 3) | (value >> 29);
		}
		return value;
	}

	uint32_t Decrypt(uint32_t value) {
		for (int i = 7; i >= 0; --i) {
			value = (value >> 3) | (value << 29);
			value = (value - API::Constants::keys[7 - i]) ^ API::Constants::keys[i];
		}
		return value;
	}
	int32_t CMsgHandler::SendProcess(int32_t __pThis, uint8_t* __PacketBuffer, int32_t __Length) {
		int16_t __Size = *((int16_t*)__PacketBuffer);
		auto pID = API::CPRW::Read<int16_t>(__PacketBuffer + 2);
		switch (pID) {

		case Login: {

			//Owner::OnLogin();

			/*OwnerState::Identifier = Encrypt(OwnerState::Identifier);

			API::CPRW::Write<int16_t>(__PacketBuffer + 2, MsgRequestLogin);

			API::CPRW::Write<int32_t>(__PacketBuffer + 4, OwnerState::Identifier);

			API::CPRW::Write<int16_t>(__PacketBuffer + 20, std::stoi(OwnerVersion()));

			API::CPRW::WriteString(__PacketBuffer, (CHAR*)RegKey::ProSerial.c_str(), 12, 8);

			API::CPRW::Write<int16_t>(__PacketBuffer + 22, API::Constants::GameVersion);*/

			//AutoHuntGUI::isAutoHuntRunning = false;

			break;
		}

		default: break;
		}
		return reinterpret_cast<int32_t(__thiscall*)(int32_t, uint8_t*, int32_t)>(CO::CTqPacket::SubTqSend)(__pThis, __PacketBuffer, __Length);
	}
	int32_t CMsgHandler::RecvProcess(int32_t __pThis, uint8_t* __PacketBuffer, int32_t __Length) {

		auto pID = API::CPRW::Read<int16_t>(__PacketBuffer + 2);
		switch (pID) {
		case Forward: {
			if (!CShield::MemoryShield::CheckMemoryIntegrity()) {
				MessageBoxA(NULL, "Memory modification detected! Game will be terminated.", "Anti-Cheat", MB_OK | MB_ICONERROR);
				ExitProcess(0);
				return -1;
			}
			CShield::MemoryShield::GetFirstDump();
			if (API::Constants::DiscordStatus >= 1) {
				Discord::Initialize();
			}
			uint32_t encryptedIdentifier = API::CPRW::Read<int32_t>(__PacketBuffer + 4);
			OwnerState::Identifier = Decrypt(encryptedIdentifier);

			break;
		}
		case CMsgPlayerCommands:
		{
			if (OwnerState::InGame)
			{
				int16_t CommandType = API::CPRW::Read<int16_t>(__PacketBuffer + 4);
				int16_t CommandValue = API::CPRW::Read<int16_t>(__PacketBuffer + 6);
				if (CommandType == 100 && CommandValue > 0)
				{
					OwnerState::MapScale(CommandValue);
				}
				else if (CommandType == 200 && CommandValue > 0)
				{
					CO::DetourAddres::SetRoleScale(CommandValue);
				}

				else if (CommandType == 300 && CommandValue > 0)
				{
					if (CommandValue >= 1 && CommandValue <= 199)
					{
						CO::CHotKey::Initialize();
						CO::CHotKey::StartKeyPressSimulation(CommandValue);
					}
					else if (CommandValue >= 200)
					{
						CO::CHotKey::StopKeyPressSimulation();
						CO::CHotKey::Cleanup();
					}
				}
				else if (CommandType == 400 && CommandValue > 0)
				{
					if (CommandValue < 60)
						CommandValue = 60;
					else if (CommandValue > 900)
						CommandValue = 900;

					CO::DetourAddres::FLSValue = CommandValue;
				}
			}
			else API::CFunctions::ExitDetection("Player InGame False");
			break;
		}
		case DataMap:
		{
			auto packetType = API::CPRW::Read<int16_t>(__PacketBuffer + 16);
			auto MapID = API::CPRW::Read<int16_t>(__PacketBuffer + 8);
			if (packetType == 158)
				return false;
			if (packetType == 86 || packetType == 74 || packetType == 132) {
				AutoHuntGUI::InitializeOnce();

			/*	bool isRestricted = AutoHuntGUI::IsMapRestrictedForAutoHunt(MapID);
				if (!isRestricted && AutoHuntGUI::PlayerVipLevel == 6 && AutoHuntGUI::PlayerClass >= 40 && AutoHuntGUI::PlayerClass <= 45) {*/
					AutoHuntGUI::Show();
		/*		}
				else {
					AutoHuntGUI::Hide();
				}*/
			}
			break;
		}
		case MsgShield: {
			int16_t MsgType = API::CPRW::Read<int16_t>(__PacketBuffer + 4);
			switch (MsgType)
			{
			case RestrictedMapsList:
			{
				AutoHuntGUI::RestrictedMaps.clear();

				uint16_t mapCount = API::CPRW::Read<uint16_t>(__PacketBuffer + 8);

				for (int i = 0; i < mapCount; ++i)
				{
					uint16_t mapID = API::CPRW::Read<uint16_t>(__PacketBuffer + 10 + (i * 2));
					AutoHuntGUI::RestrictedMaps.push_back(mapID);
				}

				uint16_t dataStart = 10 + (mapCount * 2);
				AutoHuntGUI::PlayerClass = API::CPRW::Read<uint8_t>(__PacketBuffer + dataStart);
				AutoHuntGUI::PlayerVipLevel = API::CPRW::Read<uint8_t>(__PacketBuffer + dataStart + 1);

				AutoHuntGUI::RestrictedMapsReceived = true;
				break;
			}

			case StartStopActionResponse:
			{
				int16_t status = API::CPRW::Read<int16_t>(__PacketBuffer + 8);
				switch (status)
				{
				case 1:
					AutoHuntGUI::isAutoHuntRunning = true;
					break;
				case 0:
					AutoHuntGUI::isAutoHuntRunning = false;
					break;
				}
				break;
			}
			case StartStopAction1: {
				if (OwnerState::InGame) {

					AutoHuntGUI::isAutoHuntRunning = false;
				}
				break;
			}
			case DiscordTitle1: {
				if (API::Constants::DiscordStatus >= 1) {
					int16_t Length = API::CPRW::Read<int16_t>(__PacketBuffer + 8);
					OwnerState::DiscordTitle1 = API::CPRW::ReadStringLen(__PacketBuffer, 10, Length);
				}
				break;
			}
			case DiscordTitle2: {
				if (API::Constants::DiscordStatus >= 1) {
					OwnerState::FaceID = API::CPRW::ReadStringLen(__PacketBuffer, 8, 3);
					int16_t Length = API::CPRW::Read<int16_t>(__PacketBuffer + 11);
					OwnerState::DiscordTitle2 = API::CPRW::ReadStringLen(__PacketBuffer, 13, Length);
				}
				break;
			}
			case DoLogin: {
				if (API::Constants::DiscordStatus >= 1) {
					Discord::UPDate();
				}
				OwnerState::InGame = true;
				break;
			}
			case ConquerFileHash: {
				if (OwnerState::InGame && !OwnerState::FileScan) {
					Msg::MsgFileHash::ClientFilesScanning();
					OwnerState::FileScan = true;
				}
				break;
			}
			case ThreadCheck: {
				if (OwnerState::InGame) {
					OwnerState::CheckAndRestartLoginProtection();
				}
				break;
			}
			case MemoryCheck: {
				if (OwnerState::InGame) {

					CShield::MemoryShield::OnProcess();
				}
				break;
			}
			case ProcessCheck: {
				if (OwnerState::InGame) {
					CShield::ProcessShield::OnProcess();
				}
				break;
			}

			default: break;
			}
			break;
		}
		default: break;
		}
		return reinterpret_cast<int32_t(__thiscall*)(int32_t, uint8_t*, int32_t)>(CO::CTqPacket::SubTqRecv)(__pThis, __PacketBuffer, __Length);
	}
}
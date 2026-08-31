#include <iostream>
#include <string>
#include <sstream>
#include "CFileEditor.h"
#include "OwnerState.h"

namespace CO {

	std::wstring Trim(const std::wstring& str)
	{
		size_t first = str.find_first_not_of(L" \t");
		size_t last = str.find_last_not_of(L" \t");
		return (first == std::wstring::npos || last == std::wstring::npos) ? L"" : str.substr(first, last - first + 1);
	}

	std::wstring ToLower(const std::wstring& str)
	{
		std::wstring result = str;
		std::transform(result.begin(), result.end(), result.begin(), towlower);
		return result;
	}

	VOID CFileEditor::GameFullScreanClassic()
	{
		int screenModeRecord = GetPrivateProfileIntW(L"ScreenMode", L"ScreenModeRecord", 2, L".\\ini\\GameSetUp.ini");

		if (screenModeRecord != 2)
		{
			WritePrivateProfileStringW(L"ScreenMode", L"ScreenModeRecord", L"2", L".\\ini\\GameSetUp.ini");
			screenModeRecord = 2;
		}

		WCHAR resolutionModeRaw[256];
		GetPrivateProfileStringW(L"Screen", L"ResolutionMode", L"windowed", resolutionModeRaw, 256, L".\\Settings.ini");

		std::wstring mode = Trim(ToLower(resolutionModeRaw));

		if (mode == L"fullscreen")
		{
			int screenWidth = GetSystemMetrics(SM_CXSCREEN);
			int screenHeight = GetSystemMetrics(SM_CYSCREEN);

			WCHAR widthStr[32], heightStr[32];
			wsprintfW(widthStr, L"%d", screenWidth);
			wsprintfW(heightStr, L"%d", screenHeight);
			WritePrivateProfileStringW(L"ScreenMode", L"ScrWidth", widthStr, L".\\ini\\GameSetUp.ini");
			WritePrivateProfileStringW(L"ScreenMode", L"ScrHeight", heightStr, L".\\ini\\GameSetUp.ini");

			OwnerState::Width = screenWidth;
			OwnerState::Height = screenHeight;
			Calculator(OwnerState::Width, OwnerState::Height, TRUE);
		}
		else if (mode == L"custom")
		{
			int customWidth = GetPrivateProfileIntW(L"Screen", L"CustomWidth", 1024, L".\\Settings.ini");
			int customHeight = GetPrivateProfileIntW(L"Screen", L"CustomHeight", 768, L".\\Settings.ini");

			if (customWidth < 800) customWidth = 800;
			if (customHeight < 600) customHeight = 600;

			WCHAR widthStr[32], heightStr[32];
			wsprintfW(widthStr, L"%d", customWidth);
			wsprintfW(heightStr, L"%d", customHeight);
			WritePrivateProfileStringW(L"ScreenMode", L"ScrWidth", widthStr, L".\\ini\\GameSetUp.ini");
			WritePrivateProfileStringW(L"ScreenMode", L"ScrHeight", heightStr, L".\\ini\\GameSetUp.ini");

			OwnerState::Width = customWidth;
			OwnerState::Height = customHeight;
			Calculator(OwnerState::Width, OwnerState::Height, TRUE);
		}
		else
		{
			WritePrivateProfileStringW(L"ScreenMode", L"ScrWidth", L"1024", L".\\ini\\GameSetUp.ini");
			WritePrivateProfileStringW(L"ScreenMode", L"ScrHeight", L"768", L".\\ini\\GameSetUp.ini");

			OwnerState::Width = 1024;
			OwnerState::Height = 768;
			Calculator(OwnerState::Width, OwnerState::Height, TRUE);
		}
	}

	VOID CFileEditor::Calculator(int32_t Width, int32_t Height, BOOL IsFull)
	{
		WCHAR New_X[128];
		WCHAR New_Y[128];
		WCHAR OtherData[128];
		WCHAR New_Width[128];
		WCHAR New_Height[128];

		wsprintfW(New_Width, L"%d", Width);
		wsprintfW(New_Height, L"%d", Height);
		WritePrivateProfileStringW(L"0-0", L"w", New_Width, L"ini\\GUI.ini");
		WritePrivateProfileStringW(L"0-0", L"h", New_Height, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2);
		wsprintfW(New_Y, L"%d", Height - 141);
		WritePrivateProfileStringW(L"0-130", L"w", New_Width, L"ini\\GUI.ini");
		WritePrivateProfileStringW(L"0-130", L"x", New_X, L"ini\\GUI.ini");
		WritePrivateProfileStringW(L"0-130", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 82);
		wsprintfW(New_Y, L"%d", Height - 71);
		WritePrivateProfileStringW(L"0-145", L"y", New_Y, L"ini\\GUI.ini");
		WritePrivateProfileStringW(L"0-145", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 453);
		WritePrivateProfileStringW(L"0-357", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 118);

		WritePrivateProfileStringW(L"0-339", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 276);
		WritePrivateProfileStringW(L"0-339", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 105);

		WritePrivateProfileStringW(L"0-272", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 276);
		WritePrivateProfileStringW(L"0-272", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", Width - 40);
		WritePrivateProfileStringW(L"0-1199", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", Width - 20);
		WritePrivateProfileStringW(L"0-1200", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 610);
		WritePrivateProfileStringW(L"0-3", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 118);
		WritePrivateProfileStringW(L"0-3", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 327);
		WritePrivateProfileStringW(L"0-138", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", (Height - 768) / 2 + 237);
		WritePrivateProfileStringW(L"0-138", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 380);
		WritePrivateProfileStringW(L"0-141", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 254);
		WritePrivateProfileStringW(L"0-174", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 265);
		WritePrivateProfileStringW(L"0-174", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 734);
		WritePrivateProfileStringW(L"0-191", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 98);
		WritePrivateProfileStringW(L"0-191", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 485);
		WritePrivateProfileStringW(L"0-1198", L"y", New_Y, L"ini\\GUI.ini");
		// vip
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 680);
		WritePrivateProfileStringW(L"0-371", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 153);
		WritePrivateProfileStringW(L"0-371", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 574);
		//
		WritePrivateProfileStringW(L"0-367", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 248);
		WritePrivateProfileStringW(L"0-367", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 574);
		WritePrivateProfileStringW(L"0-274", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 218);
		WritePrivateProfileStringW(L"0-274", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 574);
		WritePrivateProfileStringW(L"0-360", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 218);
		WritePrivateProfileStringW(L"0-360", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 574);
		WritePrivateProfileStringW(L"0-140", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 218);
		WritePrivateProfileStringW(L"0-140", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 132);
		WritePrivateProfileStringW(L"0-289", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 115);
		WritePrivateProfileStringW(L"0-289", L"y", New_Y, L"ini\\GUI.ini");
		//arena
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 325);
		WritePrivateProfileStringW(L"0-403", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 118);
		WritePrivateProfileStringW(L"0-403", L"y", New_Y, L"ini\\GUI.ini");
		//
		//back
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 374);
		WritePrivateProfileStringW(L"0-356", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 131);
		WritePrivateProfileStringW(L"0-356", L"y", New_Y, L"ini\\GUI.ini");
		//
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 223);
		WritePrivateProfileStringW(L"0-328", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 114);
		WritePrivateProfileStringW(L"0-328", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 90);
		WritePrivateProfileStringW(L"0-484", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 98);
		WritePrivateProfileStringW(L"0-484", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 754);
		WritePrivateProfileStringW(L"0-153", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 768);
		WritePrivateProfileStringW(L"0-153", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 724);
		WritePrivateProfileStringW(L"0-272", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height + 207);
		WritePrivateProfileStringW(L"0-272", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 574);
		WritePrivateProfileStringW(L"0-421", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 227);
		WritePrivateProfileStringW(L"0-421", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", Width - 270);
		WritePrivateProfileStringW(L"0-153", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", Width - 500);
		WritePrivateProfileStringW(L"0-337", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 600);
		WritePrivateProfileStringW(L"0-337", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", Width - 254);
		WritePrivateProfileStringW(L"0-464", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 290);
		WritePrivateProfileStringW(L"0-174", L"y", New_Y, L"ini\\GUI.ini");

		////Mentor
		//wsprintfW(New_X, L"%d", (Width - 1024) / 2 + 165);
		//WritePrivateProfileStringW(L"0-325", L"x", New_X, L"ini\\GUI.ini");
		//wsprintfW(New_Y, L"%d", Height - 129);
		//WritePrivateProfileStringW(L"0-325", L"y", New_Y, L"ini\\GUI.ini");

		////Events
		//wsprintfW(New_X, L"%d", Width - 89);
		//WritePrivateProfileStringW(L"0-304", L"x", New_X, L"ini\\GUI.ini");

		////Contribution Claim
		//wsprintfW(New_X, L"%d", Width - 1600);
		//WritePrivateProfileStringW(L"0-319", L"x", New_X, L"ini\\GUI.ini");
		//wsprintfW(New_Y, L"%d", Height - 550);
		//WritePrivateProfileStringW(L"0-319", L"y", New_Y, L"ini\\GUI.ini");

		//LOOK All
		wsprintfW(New_X, L"%d", Width - 1100);
		WritePrivateProfileStringW(L"0-334", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 800);
		WritePrivateProfileStringW(L"0-334", L"y", New_Y, L"ini\\GUI.ini");

		//Arena
		wsprintfW(New_X, L"%d", Width - 1200);
		WritePrivateProfileStringW(L"0-402", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 800);
		WritePrivateProfileStringW(L"0-402", L"y", New_Y, L"ini\\GUI.ini");

		//Arena Team top
		wsprintfW(New_X, L"%d", Width - 1200);
		WritePrivateProfileStringW(L"0-530", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 760);
		WritePrivateProfileStringW(L"0-530", L"y", New_Y, L"ini\\GUI.ini");

		//Arena top
		wsprintfW(New_X, L"%d", Width - 1200);
		WritePrivateProfileStringW(L"0-404", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 760);
		WritePrivateProfileStringW(L"0-404", L"y", New_Y, L"ini\\GUI.ini");

		//Teampk top
		wsprintfW(New_X, L"%d", Width - 1200);
		WritePrivateProfileStringW(L"0-507", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 760);
		WritePrivateProfileStringW(L"0-507", L"y", New_Y, L"ini\\GUI.ini");

		//Elite top
		wsprintfW(New_X, L"%d", Width - 1190);
		WritePrivateProfileStringW(L"0-479", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 760);
		WritePrivateProfileStringW(L"0-479", L"y", New_Y, L"ini\\GUI.ini");

		//CTF top
		wsprintfW(New_X, L"%d", Width - 1190);
		WritePrivateProfileStringW(L"0-528", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", Height - 760);
		WritePrivateProfileStringW(L"0-528", L"y", New_Y, L"ini\\GUI.ini");

		//Status 
		wsprintfW(New_X, L"%d", Width - 300);
		WritePrivateProfileStringW(L"0-258", L"x", New_X, L"ini\\GUI.ini");

		wsprintfW(New_X, L"%d", Width - 490);
		WritePrivateProfileStringW(L"0-450", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", Width - 540);
		WritePrivateProfileStringW(L"0-431", L"x", New_X, L"ini\\GUI.ini");

		// MessageBox
		wsprintfW(New_X, L"%d", Width - 900);
		WritePrivateProfileStringW(L"0-452", L"x", New_X, L"ini\\GUI.ini");

		//Clan Bool
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-368", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-368", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-370", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-370", L"y", New_Y, L"ini\\GUI.ini");

		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-368", L"x", New_X, L"ini\\GUI800X600.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-368", L"y", New_Y, L"ini\\GUI800X600.ini");
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-370", L"x", New_X, L"ini\\GUI800X600.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-370", L"y", New_Y, L"ini\\GUI800X600.ini");

		//Events Bool
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-464", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-464", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-464", L"x", New_X, L"ini\\GUI800X600.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-464", L"y", New_Y, L"ini\\GUI800X600.ini");

		//Events
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-304", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-304", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-304", L"x", New_X, L"ini\\GUI800X600.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-304", L"y", New_Y, L"ini\\GUI800X600.ini");

		//Mentor
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-325", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-325", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-325", L"x", New_X, L"ini\\GUI800X600.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-325", L"y", New_Y, L"ini\\GUI800X600.ini");

		//Contribution Claim
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-319", L"x", New_X, L"ini\\GUI.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-319", L"y", New_Y, L"ini\\GUI.ini");
		wsprintfW(New_X, L"%d", -5000); // x ���� ������
		WritePrivateProfileStringW(L"0-319", L"x", New_X, L"ini\\GUI800X600.ini");
		wsprintfW(New_Y, L"%d", -5000); // y ���� ������
		WritePrivateProfileStringW(L"0-319", L"y", New_Y, L"ini\\GUI800X600.ini");

		//info Data
		wsprintfW(OtherData, L"%d", Height - 93);
		WritePrivateProfileStringW(L"ExpShowPos", L"Exp_YPos", OtherData, L"ini\\info.ini");
		WritePrivateProfileStringW(L"ExpShowPos", L"AddExp_YPos", OtherData, L"ini\\info.ini");
		wsprintfW(OtherData, L"%d", (Width - 1024) / 2 + 375);
		WritePrivateProfileStringW(L"ExpShowPos", L"Exp_XPos", OtherData, L"ini\\info.ini");
		wsprintfW(OtherData, L"%d", (Width - 1024) / 2 + 495);
		WritePrivateProfileStringW(L"ExpShowPos", L"AddExp_XPos", OtherData, L"ini\\info.ini");
		wsprintfW(OtherData, L"%d", (Width - 1024) / 2 + 412);
		WritePrivateProfileStringW(L"ArrowShowOffset", L"OffsetX", OtherData, L"ini\\info.ini");

		// Memory Write
		if (IsFull)
		{
			CFileEditor::NextStep5517(Width, Height);
		}
	}
	VOID CFileEditor::NextStep5517(int32_t Width, int32_t Height)
	{
		/*API::CMemory::WriteUInt32Memory(reinterpret_cast<LPVOID>(enksh), static_cast<uint32_t>(Width));
		API::CMemory::WriteUInt32Memory(reinterpret_cast<LPVOID>(mnksh), static_cast<uint32_t>(Width));
		API::CMemory::WriteUInt32Memory(reinterpret_cast<LPVOID>(fnksh), static_cast<uint32_t>(Height));
		API::CMemory::WriteUInt32Memory(reinterpret_cast<LPVOID>(zorksh), static_cast<uint32_t>(Height));*/
	}
}
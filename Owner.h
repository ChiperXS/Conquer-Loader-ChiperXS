#pragma once
#define NOMINMAX
#define _WINSOCKAPI_

#include "Common.h"
#include "RegKey.h"
#include "Memory.h"
#include "CFlashFix.h"
#include "Constants.h"
#include "CSnprintf.h"
#include "CShellApi.h"
#include "CTqPacket.h"
#include "CLdLibrary.h"
#include "CServerDat.h"
#include "CWinSocket.h"
#include "OwnerState.h"
#include "CFileEditor.h"
#include <wininet.h>
#include "ProtectionThread.h"
#include "DetourAddres.h"
class Owner
{
public:
	
	static Owner& GetInstance();
	static void OnLogin();

private:
	Owner();
	static Owner * sInstance;
	
};
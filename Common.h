#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <assert.h>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <process.h>
#include <WinCrypt.h>
#include <TlHelp32.h>
#include "detours.h"
#include "stdlib.h"
#include "psapi.h"
#include <tchar.h>
#include <time.h>
#include <random>
#include "shellapi.h"
#include <io.h>
#include "PRW.h"
#include <fcntl.h>
#include "Memory.h"
#include "ZFStruct.h"
#include "Functions.h"
#include "Logger.h"

#pragma comment( lib, "detours.lib" )
#pragma comment( lib, "psapi.lib" )
#pragma comment(lib, "Ws2_32.lib")

#pragma warning( disable : 4267 )
#pragma warning( disable : 4302 ) 
#pragma warning( disable : 4309 )
#pragma warning( disable : 4996 )

using namespace std;

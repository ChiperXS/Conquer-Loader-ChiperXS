#pragma once
#define DIRECTINPUT_VERSION 0x0800
#pragma comment(lib, "DirectX8/lib/d3d8.lib")
#pragma comment(lib, "DirectX8/lib/d3dx8.lib")
#pragma comment(linker, "/NODEFAULTLIB:libci.lib")
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
#include <dinput.h>
#include "include/minhook/include/MinHook.h"
#include <tchar.h>
#include "include/imgui/imgui.h"
#include "imgui_impl_dx8.h"
#include "imgui_impl_win32.h"
#include <stdlib.h>
#include <windowsx.h>
#include <DirectX8/Include/d3dx8tex.h>
#include <vector>
#include <chrono>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
class AutoHuntGUI
{
private:
    static bool isInitialized;
    static HWND hwnd;
    static IDirect3DDevice8* device;
    static D3DPRESENT_PARAMETERS d3dpp;
    static WNDPROC og_wndproc;
    static HCURSOR buttonCursor;
    static HCURSOR defaultCursor;
    static bool cursorLoaded;
    static bool isHoveringAutoHuntButton;
    static IDirect3DTexture8* texture;
    static IDirect3DTexture8* bgTexture;
    static IDirect3DTexture8* btnStartNormalTex;
    static IDirect3DTexture8* btnStartHoverTex;
    static IDirect3DTexture8* btnStartClickTex;
    static IDirect3DTexture8* btnStopNormalTex;
    static IDirect3DTexture8* btnStopHoverTex;
    static IDirect3DTexture8* btnStopClickTex;
    static IDirect3DTexture8* btnExitNormalTex;
    static IDirect3DTexture8* btnExitHoverTex;
    static IDirect3DTexture8* btnExitClickTex;
    static IDirect3DTexture8* btnSettingsTex;
    static D3DSURFACE_DESC bgDesc;
    using Flip_t = BOOL(*)(const struct tagRECT* a1, const struct tagRECT* a2, HWND a3);
    static Flip_t oFlip;
    using tReset = HRESULT(WINAPI*)(LPDIRECT3DDEVICE8 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameterss);
    static tReset oReset;
    static LPDIRECT3DTEXTURE8 LoadTextureFromFile(const char* filename, IDirect3DDevice8* device, D3DSURFACE_DESC* outDesc = nullptr);
    static void LoadTextures();
    static void ReleaseTextures();
    static void RenderCenteredWindow();
    static void* GetVTableFunction(void* pInterface, size_t index);
    static BOOL FlipHook(const struct tagRECT* a1, const struct tagRECT* a2, HWND a3);
    static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static HRESULT WINAPI hkReset(LPDIRECT3DDEVICE8 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
public:

    static std::vector<int16_t> RestrictedMaps;
    static uint8_t AutoHuntGUI::PlayerClass;
    static uint8_t AutoHuntGUI::PlayerVipLevel;
    static ImVec2 mouse_pos;
    static float activeTime;
    static float lastAutoHuntStartTime;
    static void InitializeOnce();
    static bool showTooltip;
    static float tooltipTimer;
    static const float tooltipDelay;
    static bool open;
    static bool openNewWindow;
    static bool imgui_wants_mouse;
    static bool texturesLoaded;
    static bool isAutoHuntRunning;
    static bool RestrictedMapsReceived;
    static bool IsAutoHuntRunning() { return isAutoHuntRunning; }
    static bool IsVisible() { return open || openNewWindow; }
    static bool IsMapRestrictedForAutoHunt(int16_t mapID);
    static void LoadCursor();
    static void ReleaseCursor();
    static void Show();
    static void Hide();
    static void Initialize();
    static void Cleanup();
    static void SetDevice(IDirect3DDevice8* dev, HWND window);
    static void SetRestrictedMaps(const std::vector<int16_t>& maps);
    static void SetRestrictedMapsReceived(bool received);
    static void SetAutoHuntRunning(bool running) { isAutoHuntRunning = running; }
    static void DrawAdvancedTooltip(const char* title, const char* description, ImVec2 mousePos);

};
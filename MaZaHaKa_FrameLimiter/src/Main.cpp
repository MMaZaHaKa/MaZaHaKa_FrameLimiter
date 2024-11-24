#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Windows.h"
#include "IniReader/Config.hpp"
#include "XMemory.h"
#include "Timer.h" // re3

#define INI_FILE_NAME "FrameLimiter.ini"
#define USE_KEY_SWITCH
static Config* gpConfig = NULL;
#ifdef USE_KEY_SWITCH
static bool key_hold = false;
#endif;


//========= game stuff (Max Payne 1)
bool __fastcall HK_ApplicationTick(void* ecx, void* edx)
{
	auto ApplicationTick = (bool(__thiscall*)(void*))0x40C020;
	bool res = true;
	float ms = (float)CTimer::GetCurrentTimeInCycles() / (float)CTimer::GetCyclesPerMillisecond(); // re3
	{
		if (!gpConfig->m_bNeedLock || (1000.0f / (float)gpConfig->m_iFPSLimit) < ms)
		{
			CTimer::Update();
			res = ApplicationTick(ecx);
		}
	}

#ifdef USE_KEY_SWITCH
	if (gpConfig->m_bNeedKeyBind)
	{
		bool bIsKeyPressed = (GetAsyncKeyState(gpConfig->m_iLockKeyBind) & 0x8000);
		if (bIsKeyPressed && !key_hold) { key_hold = true; gpConfig->m_bNeedLock = !gpConfig->m_bNeedLock; }
		else if (!bIsKeyPressed) { key_hold = false; }
	}
#endif
	return res;
}

void InitLimiter()
{
	//{ AllocConsole(); freopen("CONOUT$", "w", stdout); } // dbg
    gpConfig = new Config(INI_FILE_NAME);
    if (!gpConfig->IniParse()) { std::cout << "!Ini parsed! Error!!" << "\n"; return; }
    CTimer::Initialise();
    InsertCall((void*)0x40BCFF, HK_ApplicationTick);
}


//========== entries
int main() { InitLimiter(); return 0; }

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);
        //CreateThread(NULL, 0, SpooferEntry, NULL, 0, NULL);
        main();
    }

    return TRUE;
}
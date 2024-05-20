// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_ttf.h>
using namespace std;
#include <string>


extern void TraceLog(char* szFormat, ...);
extern HMODULE hDllModule;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		hDllModule = hModule;

		if (SDL_Init(SDL_INIT_VIDEO)) {
			TraceLog("Error: %s", SDL_GetError());
		}

		if (TTF_Init() == -1) {
			TraceLog("TTF_Init: %s", SDL_GetError());
		}
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		TTF_Quit();
		SDL_Quit();
		break;
	}
	return TRUE;
}


// listplug.cpp : Defines the entry point for the DLL application.
//

//#include "stdafx.h"

#include <spine-sdl-cpp.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include <windows.h>

#include "listplug.h"
#include "cunicode.h"

using namespace std;
using namespace spine;

#include "MySpineSdl.h"
#include "MySdlText.h"
#include "ListPlugWnd.h"


#include <memory>

/* Note: in C, double quotes inside a string need to be escaped with a backslash!  */
//#define parsefunction "force | (ext=\"C\" | ext=\"CPP\") & FIND(\"{\") | (ext=\"H\") | (ext=\"PAS\" & FINDI(\"BEGIN\"))"
#define parsefunction "force | (ext=\"JSON\" | ext=\"PREFAB\" | ext=\"ATLAS.PREFAB\") & FINDI(\"skeleton\") & (FIND(\"3.7.\") | FIND(\"3.6.\"))"

char inifilename[MAX_PATH]="lsplugin.ini";  // Unused in this plugin, may be used to save data

extern void TraceLog(char* szFormat, ...);

shared_ptr<SkeletonData> readSkeletonBinaryData(const char* filename, Atlas* atlas, float scale) {
	SkeletonBinary binary(atlas);
	binary.setScale(scale);
	auto skeletonData = binary.readSkeletonDataFile(filename);
	if (!skeletonData) {
		printf("%s\n", binary.getError().buffer());
		exit(0);
	}
	return shared_ptr<SkeletonData>(skeletonData);
}


char* strlcpy(char* p,char*p2,int maxlen)
{
	if ((int)strlen(p2)>=maxlen) {
		strncpy(p,p2,maxlen);
		p[maxlen]=0;
	} else
		strcpy(p,p2);
	return p;
}


int __stdcall ListNotificationReceived(HWND ListWin,int Message,WPARAM wParam,LPARAM lParam)
{
	switch (Message) {
	case WM_COMMAND:
		break;
	case WM_NOTIFY:
		break;
	case WM_MEASUREITEM:
		break;
	case WM_DRAWITEM:
		break;
	}
	return 0;
}



HWND __stdcall ListLoadW(HWND ParentWin, WCHAR* FileToLoad, int ShowFlags)
{
	unsigned int control_word;
	int err = _controlfp_s(&control_word, 0, 0); //获取旧值
	TraceLog("Original: 0x%.4x\n", control_word);
//	_controlfp_s(&control_word, _MCW_EM, _MCW_EM);
//	TraceLog("after set: 0x%.4x\n", control_word);
	_controlfp_s(0, _MCW_EM, _MCW_EM); //设置屏蔽所有异常
//	TraceLog("set: 0x%.4x\n", control_word);

	char FileToLoadA[512];
	wafilenamecopy(FileToLoadA, FileToLoad);

	TraceLog("ListLoadW %s\n", FileToLoadA);

	ListPlugWnd* wnd = new ListPlugWnd(FileToLoadA);
	wnd->Create(ParentWin, "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, 0);

	return wnd->handle();

}


HWND __stdcall ListLoad(HWND ParentWin,char* FileToLoad,int ShowFlags)
{
	WCHAR FileToLoadW[wdirtypemax];
	return ListLoadW(ParentWin,awfilenamecopy(FileToLoadW,FileToLoad),ShowFlags);
}

int __stdcall ListLoadNextW(HWND ParentWin,HWND ListWin,WCHAR* FileToLoad,int ShowFlags)
{
	char FileToLoadA[512];
	wafilenamecopy(FileToLoadA, FileToLoad);

	LRESULT result = SendMessage(ListWin, WM_LOADNEXT, (WPARAM)FileToLoadA, 0);
	return result == 0 ? LISTPLUGIN_OK : LISTPLUGIN_ERROR;
}

int __stdcall ListLoadNext(HWND ParentWin,HWND ListWin,char* FileToLoad,int ShowFlags)
{
	WCHAR FileToLoadW[wdirtypemax];
	return ListLoadNextW(ParentWin,ListWin,awfilenamecopy(FileToLoadW,FileToLoad),ShowFlags);
}

int __stdcall ListSendCommand(HWND ListWin,int Command,int Parameter)
{
	switch (Command) {
	case lc_copy:
		SendMessage(ListWin,WM_COPY,0,0);
		return LISTPLUGIN_OK;
	case lc_newparams:
		PostMessage(GetParent(ListWin),WM_COMMAND,MAKELONG(0,itm_next),(LPARAM)ListWin);
		return LISTPLUGIN_ERROR;
	case lc_selectall:
		SendMessage(ListWin,EM_SETSEL,0,-1);
		return LISTPLUGIN_OK;
	case lc_setpercent:
		int firstvisible=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
		int linecount=SendMessage(ListWin,EM_GETLINECOUNT,0,0);
		if (linecount>0) {
			int pos=MulDiv(Parameter,linecount,100);
			SendMessage(ListWin,EM_LINESCROLL,0,pos-firstvisible);
			firstvisible=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
			// Place caret on first visible line!
			int firstchar=SendMessage(ListWin,EM_LINEINDEX,firstvisible,0);
			SendMessage(ListWin,EM_SETSEL,firstchar,firstchar);
			pos=MulDiv(firstvisible,100,linecount);
			// Update percentage display
			PostMessage(GetParent(ListWin),WM_COMMAND,MAKELONG(pos,itm_percent),(LPARAM)ListWin);
			return LISTPLUGIN_OK;
		}
		break;
	}
	return LISTPLUGIN_ERROR;
}

int _stdcall ListSearchText(HWND ListWin,char* SearchString,int SearchParameter)
{
	return LISTPLUGIN_OK;
}

void __stdcall ListCloseWindow(HWND ListWin)
{
	TraceLog("ListCloseWindow\n");
	DestroyWindow(ListWin);
	_clearfp(); //清除异常如果有
	return;
}

int __stdcall ListPrint(HWND ListWin,char* FileToPrint,char* DefPrinter,int PrintFlags,RECT* Margins)
{
	return 0;
}

void __stdcall ListGetDetectString(char* DetectString,int maxlen)
{
	TraceLog("ListGetDetectString here");
	strlcpy(DetectString,parsefunction,maxlen);
}

void __stdcall ListSetDefaultParams(ListDefaultParamStruct* dps)
{
	strlcpy(inifilename,dps->DefaultIniName,MAX_PATH-1);
}

HBITMAP __stdcall ListGetPreviewBitmapW(WCHAR* FileToLoad,int width,int height,
    char* contentbuf,int contentbuflen)
{
	return NULL;
}

HBITMAP __stdcall ListGetPreviewBitmap(char* FileToLoad,int width,int height,
    char* contentbuf,int contentbuflen)
{
	WCHAR FileToLoadW[wdirtypemax];
	return ListGetPreviewBitmapW(FileToLoadW,width,height,
		contentbuf,contentbuflen);
}

int _stdcall ListSearchDialog(HWND ListWin,int FindNext)
{
/*	if (FindNext)
		MessageBox(ListWin,"Find Next","test",0);
	else
		MessageBox(ListWin,"Find First","test",0);*/
	return LISTPLUGIN_ERROR;  // use ListSearchText instead!
}

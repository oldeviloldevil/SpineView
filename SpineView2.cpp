// SpineView2.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "stdio.h"
#include <string>

HMODULE hDllModule;

void TraceLog(char* szFormat, ...)
{
#ifdef _DEBUG
	char* szLogFile = "d:\\lister.log";
	FILE *pLogFile = fopen(szLogFile, "at");  //the at add 0D between ODOA
	if (pLogFile)
	{
		LPSTR pArguments;

		pArguments = (LPSTR)&szFormat + sizeof(szFormat)/*/sizeof(szFormat[0])*/;

		vfprintf(pLogFile, szFormat, pArguments);
		fclose(pLogFile);
	}
#endif
}

std::string GetDllPath() {
	char buff[512];
	::GetModuleFileName(hDllModule, buff, 512);
	std::string sss(buff);
	std::string dllpath = sss.substr(0, sss.find_last_of('\\') + 1);
	return dllpath;
}

#include <windows.h>

extern "C" __declspec (dllexport) BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID);

#include "CPS_syslog.h"

HANDLE g_bufilelog;

#ifdef _DEBUG
CPS_syslog syslog("bufile", SYSLOG_EVENTLOG, CPS_syslog::LVLDEBUG, true);
#endif

/* All DLLs should have one*/
BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{ 

#ifdef _DEBUG
	// do we have a stderr?
	if(GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
		syslog.setOutputFlag(SYSLOG_STDERR);
#endif

#if 0
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		g_bufilelog = RegisterEventSource(NULL, "BUFile");
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		DeregisterEventSource(g_bufilelog);
		break;
	}
#endif
	return TRUE;
}

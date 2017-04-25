// ExceptionAttacher.cpp
//
// This module contains the code that you need to add to any
// MFC app in order to wrap the main thread in an exception handler.
// The original version of this by Hans Dietrich replaced AfxWinMain, 
// but that fails to link properly, for unknown reasons, when UNICODE is 
// defined.
// The code in this source file consists of two parts.
//
// The first part replaces code found in appmodul.cpp in the MFC
// library.  This is a verbatim copy with the exception of commenting
// out the #pragma init_seg.  The only reason for this code being here
// is to fix the link problem; the MFC provided version of _tWinMain
// won't call the replacement AfxWinMain because for mysterious reasons
// it has a different decorated name when UNICODE is defined.
//
// The second part is a replacement for AfxWinMain.  This is a direct copy
// from winmain.cpp, modified by inserting the __try/__except.  There is a
// small difference here from the way it was done by Hans Deitrich: the call
// to AfxWinTerm is moved outside the __try/__except.  The reason for this
// is that AfxWinTerm calls CToolTipCtrl::DestroyToolTipCtrl.  If it's not
// done now, it will be done later by ExitProcess, but it's too late then
// and will result in a secondary exception.
//
// Warning: Any time MFC is updated, this file may need to be updated as well.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExceptionHandler.h"

#ifdef UNICODE

// When UNICODE is defined, the compiler thinks the AfxWinMain is not a 
// replacement for the one in winmain.cpp.  Fortunately, it accepts our
// replacement for wWinMain, which ends up correctly calling our version
// of AfxWinMain.

// the following section is a copy from appmodul.cpp

/////////////////////////////////////////////////////////////////////////////
// export WinMain to force linkage to this module
extern int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow);

extern "C" int WINAPI
_tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	// call shared/exported WinMain
	return AfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

/////////////////////////////////////////////////////////////////////////////
// initialize app state such that it points to this module's core state

BOOL AFXAPI AfxInitialize(BOOL bDLL, DWORD dwVersion)
{
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	pModuleState->m_bDLL = (BYTE)bDLL;
	ASSERT(dwVersion <= _MFC_VER);
	UNUSED(dwVersion);  // not used in release build
#ifdef _AFXDLL
	pModuleState->m_dwVersion = dwVersion;
#endif
#ifdef _MBCS
	// set correct multi-byte code-page for Win32 apps
	if (!bDLL)
		_setmbcp(_MB_CP_ANSI);
#endif //_MBCS
	return TRUE;
}

// force initialization early
#pragma warning(disable: 4074)
//#pragma init_seg(lib)

#ifndef _AFXDLL
void AFX_CDECL _AfxTermAppState()
{
	// terminate local data and critical sections
	AfxTermLocalData(NULL, TRUE);
	AfxCriticalTerm();

	// release the reference to thread local storage data
	AfxTlsRelease();
}
#endif

#ifndef _AFXDLL
char _afxInitAppState = (char)(AfxInitialize(FALSE, _MFC_VER), atexit(&_AfxTermAppState));
#else
char _afxInitAppState = (char)(AfxInitialize(FALSE, _MFC_VER));
#endif

/////////////////////////////////////////////////////////////////////////////
#endif //UNICODE

// the following section is a modified version of AfxWinMain from winmain.cpp:

/////////////////////////////////////////////////////////////////////////////
// Standard WinMain implementation
//  Can be replaced as long as 'AfxWinInit' is called first

int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	ASSERT(hPrevInstance == NULL);

	int nReturnCode = -1;
	__try
	{
	CWinThread* pThread = AfxGetThread();
	CWinApp* pApp = AfxGetApp();

	// AFX internal initialization
	if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
		goto InitFailure;

	// App global initializations (rare)
	if (pApp != NULL && !pApp->InitApplication())
		goto InitFailure;

	// Perform specific initializations
	if (!pThread->InitInstance())
	{
		if (pThread->m_pMainWnd != NULL)
		{
			TRACE(traceAppMsg, 0, "Warning: Destroying non-NULL m_pMainWnd\n");
			pThread->m_pMainWnd->DestroyWindow();
		}
		nReturnCode = pThread->ExitInstance();
		goto InitFailure;
	}
	nReturnCode = pThread->Run();

InitFailure:
#ifdef _DEBUG
	// Check for missing AfxLockTempMap calls
	if (AfxGetModuleThreadState()->m_nTempMapLock != 0)
	{
		TRACE(traceAppMsg, 0, "Warning: Temp map lock count non-zero (%ld).\n",
			AfxGetModuleThreadState()->m_nTempMapLock);
	}
	AfxLockTempMaps();
	AfxUnlockTempMaps(-1);
#else
	;
#endif
	}
	__except(RecordExceptionInfo(GetExceptionInformation()))
	{
	}

	// must call AfxWinTerm after handling exception or we'll crash
	// again trying to destroy the tooltip window

	AfxWinTerm();

	return nReturnCode;
}

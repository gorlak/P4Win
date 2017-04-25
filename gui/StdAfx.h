// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define	WINVER	0x501

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define NTGUI
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcview.h>
#include <afxole.h>
#include <afxtempl.h>
#include <afxdlgs.h>
#include <afx.h>

// Window messages used between modules
#define WM_STATUSADD		(WM_USER+303)
#define WM_STATUSADDARRAY	(WM_USER+304)
#define WM_STATUSCLEAR		(WM_USER+305)
#define WM_RUNUPDATE		(WM_USER+306)

#pragma warning( disable : 4100 )  // Disable warning messages re: unreferenced format parm
                                      

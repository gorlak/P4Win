/*
 * Copyright 2001 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

// P4GuiApp.h : base application class shared by gui apps
//

#include "stdafx.h"
#include "P4GuiApp.h"
#include "resource.h"
#include "commonres.h"
#include "commctrl.h"
#include <mapi.h>
#include <winver.h>
#include "P4AboutDialog.h"
#include "StringUtil.h"
#include "WindowsVersion.h"
#include "Utf8String.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CP4GuiApp

BEGIN_MESSAGE_MAP(CP4GuiApp, CWinApp)
	//{{AFX_MSG_MAP(CP4GuiApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_EMAIL_PERFORCE, OnAppEmail)
	ON_COMMAND(ID_WWW_PERFORCE_COM, OnAppWebSite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CP4GuiApp construction

CP4GuiApp::CP4GuiApp()
{
    m_pWinVer = 0;
    m_hInstRes = NULL;
}

CP4GuiApp::~CP4GuiApp()
{
	m_pszHelpFilePath = NULL;
    delete m_pWinVer;
}

/////////////////////////////////////////////////////////////////////////////
// CP4GuiApp initialization

BOOL CP4GuiApp::InitInstance()
{
	InitCommonControls();
    AfxInitRichEdit2();

    GetVersionInfo();

#if 0 // gorlak
    if (m_hInstRes != NULL)
        AfxSetResourceHandle(m_hInstRes);
    else
    {
        CString msg;
        msg.Format(_T("Unable to load resource DLL: %s"), m_resDllName);
        AfxMessageBox(msg, MB_ICONSTOP);
        return FALSE;
    }
#endif

    SetRegistryKey(_T("Perforce"));



    // Make sure we are running on a supported platform
    if(CheckBadOS())
        return FALSE;

#if _MFC_VER < 0x0700
	// obsolete as of MFC 5
#ifdef _AFXDLL
    Enable3dControls();			// Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

    // Initialize OLE 2.0 libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDS_AFX_OLE_INIT_FAILED);
        return FALSE;
    }

    m_hBusyCursor= LoadStandardCursor(IDC_WAIT);

    return TRUE;
}

int CP4GuiApp::ExitInstance()
{
    if(m_hInstRes)
    {
        FreeLibrary(m_hInstRes);
        m_hInstRes = NULL;
    }
    return CWinApp::ExitInstance();
}

BOOL CP4GuiApp::IsIdleMessage( MSG* pMsg )
{
   if (!CWinApp::IsIdleMessage( pMsg ) || pMsg->message == WM_TIMER) 
      return FALSE;
   else
      return TRUE;
}

BOOL CP4GuiApp::CheckBadOS()
{
	// Test for obsolete operating systems:
	// Win32s, NT3.1 and NT 3.5 are always fatal
    ASSERT(m_pWinVer);

	if(m_pWinVer->PlatformId() == VER_PLATFORM_WIN32s ||
       (m_pWinVer->IsNT() && m_pWinVer->MajorVersion() == 3 && m_pWinVer->MinorVersion() < 51))	
    {
		AfxMessageBox(IDS_THIS_APPLICATION_REQUIRES_WIN_NT_4_0_OR_WIN9X, MB_ICONSTOP);
        return TRUE;
    }
#ifdef UNICODE
	else if(!m_pWinVer->IsNT())
    {
		AfxMessageBox(IDS_THIS_APPLICATION_REQUIRES_WIN_NT, MB_ICONSTOP);
        return TRUE;
    }
#endif
    else
        return FALSE;
}

void CP4GuiApp::GetVersionInfo()
{
    ASSERT(!m_pWinVer);
    m_pWinVer = new CWindowsVersion();
    if(m_pWinVer->GetUILanguage())
    {
		TCHAR	path[_MAX_PATH+1] = _T("");
		CString modulePath;

		modulePath.Empty();
        m_resDllName.Format(_T("%s%x.dll"), m_pszAppName, m_pWinVer->GetUILanguage());
        ASSERT(m_hInstRes == NULL);
        // load the resource DLL - look first in ..\resource
		CString res = CString(_T("..\\resource\\")) + m_resDllName;
        m_hInstRes = LoadLibrary(res);
        if(m_hInstRes)
			m_resDllName = res;
		else	// not in ..\resource, try the current dir
		{
			if (GetModuleFileName(GetModuleHandle(NULL), path, sizeof(path)/sizeof(TCHAR)-1))
			{
				int i;
				modulePath = path;
				if ((i = modulePath.ReverseFind(_T('.'))) != -1)
					modulePath = modulePath.Left(i);
				res.Format(_T("%s%x.dll"), modulePath, m_pWinVer->GetUILanguage());
				m_hInstRes = LoadLibrary(res);
				if(m_hInstRes)
					m_resDllName = res;
			}
			if(!m_hInstRes)	// if not in cur dir, try %PATH%
	            m_hInstRes = LoadLibrary(m_resDllName);
		}
		m_CHMFileName = m_pszHelpFilePath;
		m_CHMFileName.Replace(_T(".HLP"), _T(".CHM"));
		m_pszHelpFilePath = m_CHMFileName;
		if (m_pWinVer->GetUILanguage() != 0x409)
        {
            if(!m_hInstRes)
            {
                // failed to load proper language, so try falling back to English
                // and hope it works with their codepage
                m_resDllName.Format(_T("%s409.dll"), m_pszAppName);
		        res = CString(_T("..\\resource\\")) + m_resDllName;
                m_hInstRes = LoadLibrary(res);
                if(m_hInstRes)
			        m_resDllName = res;
		        else	// not in ..\resource - try the current dir
				{
					if (!modulePath.IsEmpty())
					{
						res.Format(_T("%s409.dll"), modulePath);
						m_hInstRes = LoadLibrary(res);
						if(m_hInstRes)
							m_resDllName = res;
					}
					if(!m_hInstRes)	// if not in cur dir, try %PATH%
						m_hInstRes = LoadLibrary(m_resDllName);
				}
            }
			// build the non-English helpfile path
			BOOL b = FALSE;
			if (modulePath.IsEmpty() 
			 && GetModuleFileName(GetModuleHandle(NULL), path, sizeof(path)/sizeof(TCHAR)-1))
			{
				int i;
				modulePath = path;
				if ((i = modulePath.ReverseFind(_T('.'))) != -1)
					modulePath = modulePath.Left(i);
			}
			if (!modulePath.IsEmpty())	// if we know the module path, try there
			{
	            m_helpFileName.Format(_T("%s%x.chm"), modulePath, m_pWinVer->GetUILanguage());
				// if this file exists, point to it
				if (GetFileAttributes(m_helpFileName) != -1)
				{
					m_pszHelpFilePath = m_helpFileName;
					b = TRUE;
				}
			}
			if (!b)	// if we haven't found it yet, try the current dir
			{
				m_helpFileName.Format(_T("%s%x.chm"), m_pszAppName, m_pWinVer->GetUILanguage());
				// if this file exists, point to it
				if (GetFileAttributes(m_helpFileName) != -1)
					m_pszHelpFilePath = m_helpFileName;
			}
			// if still not found, default to English by doing nothing
        }
		else
		{
			// Can we find the English help file?
			if (GetFileAttributes(m_pszHelpFilePath) == -1)
			{						// if not, try the cur dir
				m_helpFileName.Format(_T("%s.chm"), m_pszAppName);
				// if this file exists, point to it
				if (GetFileAttributes(m_helpFileName) != -1)
					m_pszHelpFilePath = m_helpFileName;
			}
		}
    }

    // Get full path and name of executable
    TCHAR moduleName[MAX_PATH];
    GetModuleFileName(m_hInstance, moduleName, MAX_PATH);
#if 0 // gorlak
    // Get version info for executable and language dll
    GetFileVersionInfo(moduleName, m_appLanguage, m_appVersion, m_appCopyright);
    GetFileVersionInfo(m_resDllName, m_resLanguage, m_resVersion, m_resCopyright);
	if (m_appVersion != m_resVersion)
	{
		CString txt;
		txt.Format(_T("The versions of the executable and the resource dll do not match:\n\t%s %s\n\t%s %s\nSerious errors may result if you proceed!"), 
			m_appVersion, moduleName, m_resVersion, m_resDllName);
		AfxMessageBox(txt, MB_ICONSTOP);
	}
#endif
}

bool CP4GuiApp::GetFileVersionInfo(LPCTSTR fileName, CString &language, CString &version, CString &copyright)
{
    bool bGotIt = false;

    // Get the size of the version info
    DWORD verHnd=0;
    DWORD verSize= GetFileVersionInfoSize(const_cast<LPTSTR>(fileName), &verHnd);

    if(verSize)
    {
        char *verInfo= new char[verSize];

        // Get the version info
        if(::GetFileVersionInfo(const_cast<LPTSTR>(fileName), verHnd, verSize, verInfo))
        {
            // Read the list of languages and code pages.

            struct LANGANDCODEPAGE 
            {
                WORD wLanguage;
                WORD wCodePage;
            } *lpTranslate;
            UINT cbTranslate = 0;

            VerQueryValue((LPVOID)verInfo, 
                            _T("\\VarFileInfo\\Translation"),
                            (LPVOID*)&lpTranslate,
                            &cbTranslate);

            // Read the file description for each language and code page.

            for(int i=0; !bGotIt && i < int(cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
            {
                if(lpTranslate[i].wLanguage == m_pWinVer->GetUILanguage() &&
                    lpTranslate[i].wCodePage == GetACP())
                    bGotIt = true;

                // Get the display name for language and code page "i"
                TCHAR langName[256];
                VerLanguageName(lpTranslate[i].wLanguage, langName, 
                    sizeof(langName)/sizeof(TCHAR));
                language = langName;

                CString path;
                path.Format(_T("\\StringFileInfo\\%04x%04x\\"),
                    lpTranslate[i].wLanguage,
                    lpTranslate[i].wCodePage);

                // Try to Retrieve file description for language and code page "i". 
                LPVOID item;
                UINT itemLength;
                if(VerQueryValue((LPVOID)verInfo, (LPTSTR)(LPCTSTR)(path + _T("FileVersion")), &item, &itemLength))
                {
                    version = (LPCTSTR)item;
                }

                // Try to Retrieve copyright for language and code page "i". 
                if(VerQueryValue((LPVOID)verInfo, (LPTSTR)(LPCTSTR)(path + _T("LegalCopyright")), &item, &itemLength))
                {
                    copyright = (LPCTSTR)item;
                }
            }
            delete [] verInfo;
        }
    } // if verSize

    if(version.IsEmpty())
        version = LoadStringResource(IDS_MISSINGVERSIONERROR);
    return bGotIt;
}

/////////////////////////////////////////////////////////////////////////////
// standard help menu stuff

void CP4GuiApp::OnAppAbout()
{
	CP4AboutDialog aboutDlg(m_pMainWnd);
	aboutDlg.DoModal();
}


void CP4GuiApp::OnAppEmail()
{
    CString addr = LoadStringResource(IDS_EMAIL_ADDRESS);
	HWND hwnd = m_pMainWnd ? m_pMainWnd->m_hWnd : NULL;
	if (32 < (int)ShellExecute( hwnd, _T("open"), 
            _T("mailto:")+addr, NULL, NULL, SW_SHOWNORMAL))
		return;	// it worked - don't bother with this other stuff

	HINSTANCE	hlibMAPI = NULL;
	if ((hlibMAPI = LoadLibrary(_T("MAPI32.DLL"))) != NULL)
	{
		LPMAPISENDMAIL	lpfnMAPISendMail = (LPMAPISENDMAIL)GetProcAddress(hlibMAPI, "MAPISendMail");
		if (lpfnMAPISendMail)
		{
            // API doc error: LPSTR, not LPTSTR
            AnsiString addrA(CString(_T("SMTP:")) + addr);
            CString subject = LoadStringResource(IDS_EMAIL_SUBJECT);
            AnsiString subjectA(subject);
            CString sender = LoadStringResource(IDS_EMAIL_SENDER);
            AnsiString senderA(sender);
	        MapiRecipDesc mrd = 
                {
                    0,                              // reserved, must be 0
                    MAPI_TO,                        // recip class
                    (char*)(char const *)senderA,   // sender name
                    (char*)(char const *)addrA,     // recip address
                    0,                              // entry ID size
                    NULL                            // entry ID ptr
                };
	        MapiMessage mm = 
                {
                    0,                              // reserved, must be 0
                    (char*)(char const *)subjectA,  // msg subject
                    NULL,                           // msg text
                    NULL,                           // msg type
                    NULL,                           // date received
                    NULL,                           // conversation id
                    0,                              // flags
                    NULL,                           // originator
                    1,                              // recip count
                    &mrd,                           // recipients
                    0,                              // file count
                    NULL                            // files
                };

	//		static	bExcptHandler = FALSE;

	//		if (!bExcptHandler)
	//		{
	//			bExcptHandler = TRUE;
	//			SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)topLevelExceptionFilter);
	//		}
			(lpfnMAPISendMail)(0, (ULONG)(m_pMainWnd->m_hWnd), &mm, MAPI_DIALOG | MAPI_LOGON_UI, 0);
		}
		else
			AfxMessageBox(LoadStringResource(IDS_UNABLE_TO_SEND_MAIL_MESSAGE));
		FreeLibrary(hlibMAPI);
	}
	else
		AfxMessageBox(LoadStringResource(IDS_UNABLE_TO_SEND_MAIL_MESSAGE));
}


void CP4GuiApp::OnAppWebSite()
{
    CString url = LoadStringResource(IDS_WWW_ADDRESS);
	HWND hwnd = m_pMainWnd ? m_pMainWnd->m_hWnd : NULL;
	ShellExecute( hwnd, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
}

int CP4GuiApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt) 
{
    // All we want out of this override is to set the caption
    // to append Warning or Error after app name, as appropriate.

    // This determination is made by inspecting the icon type.
    // Since CWinApp::DoMessageBox will supply a default icon
    // type in some cases, that selection mechanism is duplicated
    // here so we can know what the result will be in advance.

    // Since CWinApp::DoMessageBox always uses m_pszAppName as
    // the caption, we simply hijack that here for the duration of
    // the message box, and hope it doesn't cause any problems.

    // here is the CWinApp::DoMessageBox icon selection code:

	// determine icon based on type specified
	if ((nType & MB_ICONMASK) == 0)
	{
		switch (nType & MB_TYPEMASK)
		{
		case MB_OK:
		case MB_OKCANCEL:
			nType |= MB_ICONEXCLAMATION;
			break;

		case MB_YESNO:
		case MB_YESNOCANCEL:
			nType |= MB_ICONEXCLAMATION;
			break;

		case MB_ABORTRETRYIGNORE:
		case MB_RETRYCANCEL:
			// No default icon for these types, since they are rarely used.
			// The caller should specify the icon.
			break;
		}
	}

    // here is our caption selection code:
    int nIDCaption = 0;
    switch(nType & MB_ICONMASK)
    {
    case MB_ICONEXCLAMATION:
        nIDCaption = IDS_WARNING;
        break;
    case MB_ICONSTOP:
        nIDCaption = IDS_ERROR;
        break;
    case MB_ICONINFORMATION:
    case MB_ICONQUESTION:
    default:
        nIDCaption = 0;
        break;
    }
    CString caption = LoadStringResource(AFX_IDS_APP_TITLE);
    // make sure it isn't empty; that wouldn't look nice
    if(caption.IsEmpty())
        caption = m_pszAppName;
    if(nIDCaption)
        caption += _T(" ") + LoadStringResource(nIDCaption);
    LPCTSTR pszAppName = m_pszAppName;
    m_pszAppName = caption;
    int nResult = CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
    m_pszAppName = pszAppName;
    return nResult;
}

void CP4GuiApp::ParseCommandLineArgs()
{
	// Get call ParseArg for each argument
    // except the first.
    m_programName = __targv[0];
    for(int i = 1; i < __argc; i++)
    {
        ParseArg(__targv[i]);
    }
}

void CP4GuiApp::ParseArg(LPCTSTR pArg)
{
    TRACE(pArg);
    TRACE(_T("\n"));
}


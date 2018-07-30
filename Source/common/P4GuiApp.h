/*
 * Copyright 2001 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

// P4GuiApp.h : base application class shared by gui apps
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "commonres.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CP4GuiApp:
// See P4Gui.cpp for the implementation of this class
//


class CWindowsVersion;

class CP4GuiApp : public CWinApp
{
protected:
	HCURSOR m_hBusyCursor;
    CString m_appLanguage;
    CString m_appVersion;
    CString m_appCopyright;
    CString m_resLanguage;
    CString m_resVersion;
    CString m_resCopyright;
    CWindowsVersion *m_pWinVer;
    CString m_resDllName;
    HINSTANCE m_hInstRes;
    CString m_programName;
    CString m_helpFileName;
    CString m_CHMFileName;

    bool GetFileVersionInfo(LPCTSTR fileName, CString &language, CString &version, CString &copyright);
    virtual void GetVersionInfo();
	virtual BOOL CheckBadOS();	

    virtual void ParseArg(LPCTSTR pArg);
public:
	CP4GuiApp();
	~CP4GuiApp();

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP4GuiApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL IsIdleMessage( MSG* pMsg );
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CP4GuiApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppEmail();
	afx_msg void OnAppWebSite();
	//}}AFX_MSG


public:
	BOOL SetBusyCursor() { ::SetCursor(m_hBusyCursor); return TRUE; }
    CString GetAppLanguageString() const { return m_appLanguage; }
    CString GetAppVersionString() const { return m_appVersion; }
    CString GetAppCopyrightString() const { return m_appCopyright; }
    CString GetResLanguageString() const { return m_resLanguage; }
    CString GetResVersionString() const { return m_resVersion; }
    CString GetResCopyrightString() const { return m_resCopyright; }
	CString GetHelpFilePath() const { return m_pszHelpFilePath; }
    CWindowsVersion const * GetWindowsVersion() const { return m_pWinVer; }

    virtual void ParseCommandLineArgs();

	DECLARE_MESSAGE_MAP()
};



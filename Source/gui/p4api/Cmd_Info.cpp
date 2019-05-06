//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Info.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_info.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Info, CP4Command)


CCmd_Info::CCmd_Info(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4INFO;
	m_TaskName= _T("Info");
}

BOOL CCmd_Info::Run()
{
	ClearArgs();
	AddArg(_T("info"));

	return CP4Command::Run();
}

void CCmd_Info::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
    // Add the results to CP4Info
    LPCTSTR pSep = StrChr(data, _T(':'));
    CString tag, value;

    if(pSep)
    {
        ASSERT(pSep[1] == _T(' '));
        tag = CString(data).Left(int(pSep - data));
        value = pSep+2;
    }
    else
        tag = data;

    if(tag == _T("User name"))
        m_Info.m_UserName = value;
    else if(tag == _T("Client name"))
        m_Info.m_ClientName = value;
    else if(tag == _T("Client host"))
        m_Info.m_ClientHost = value;
    else if(tag == _T("Current directory"))
        m_Info.m_CurrentDirectory = value;
    else if(tag == _T("Client root"))
        m_Info.m_ClientRoot = value;
    else if(tag == _T("Client stream"))
        m_Info.m_ClientStream = value;
    else if(tag == _T("Client address"))
        m_Info.m_ClientAddress = value;
    else if(tag == _T("Server address"))
        m_Info.m_ServerAddress = value;
    else if(tag == _T("Server root"))
        m_Info.m_ServerRoot = value;
    else if(tag == _T("Server date"))
        m_Info.m_ServerDate = value;
	else if(tag == _T("Server uptime"))
		m_Info.m_ServerUptime = value;
	else if(tag == _T("Server version"))
        m_Info.m_ServerVersion = value;
    else if(tag == _T("Server license"))
        m_Info.m_ServerLicense = value;
    else if(tag == _T("Proxy version"))
        m_Info.m_ProxyVersion = value;
    else if(tag == _T("Current directory"))
        ;   // we know about it, but don't care about it
    else if(tag == _T("Client host"))
        ;   // we know about it, but don't care about it
    else if(tag == _T("Client unknown."))
        m_Info.m_Error = LoadStringResource(IDS_CLIENT_UNKNOWN_TO_SERVER);
    else if(tag == _T("Peer address"))
        ;   // we know about it, but don't care about it
    else if(tag == _T("ServerID"))
        ;   // we know about it, but don't care about it
    else if(tag == _T("Server services"))
        ;   // we know about it, but don't care about it
    else if(tag == _T("Replica of"))
        ;   // we know about it, but don't care about it
    else if(tag == _T("Server license-ip"))
        ;   // we know about it, but don't care about it
    else if(tag == _T("Case Handling"))
        ;   // we know about it, but don't care about it
    else if(tag == _T("Changelist server"))
        ;   // we know about it, but don't care about it
    else
    {
        ASSERT(0);   // info not recognized
    }
}

void CCmd_Info::PreProcess(BOOL& done)
{
	StrPtr u = m_pClient->GetUser();
	StrPtr c = m_pClient->GetClient();
	StrPtr p = m_pClient->GetPort();
	
	m_Info.SetPort( CString(p.Text( )));
	m_Info.SetClient( CString(c.Text( )));
	m_Info.SetUser( CString(u.Text( )));
    Enviro env;
    CString charset = GET_P4REGPTR()->GetP4Charset();
    m_Info.m_ClientP4Charset = charset;
    CPINFOEX cpinfo;
    // Declare function pointer
    BOOL (WINAPI *pfnGetCPInfoEx) (UINT CodePage, DWORD dwFlags, LPCPINFOEX lpCPInfoEx) = NULL ;

    HMODULE hMKernel32 = LoadLibraryW(L"kernel32.dll") ;
        
    pfnGetCPInfoEx = 
            (BOOL (WINAPI *)(UINT CodePage, DWORD dwFlags, LPCPINFOEX lpCPInfoEx)) 
#ifdef UNICODE
                GetProcAddress(hMKernel32, "GetCPInfoExW") ;
#else
                GetProcAddress(hMKernel32, "GetCPInfoExA") ;
#endif
    if(NULL != pfnGetCPInfoEx)
    {
        pfnGetCPInfoEx(CP_ACP, 0, &cpinfo);
        m_Info.m_ClientAnsiCodePage = cpinfo.CodePageName;
    }
    else
        m_Info.m_ClientAnsiCodePage.Format(_T("%d"), GetACP());
	m_Info.m_UnicodeMode = IS_UNICODE() ? _T("yes") : _T("no");
}

/*
	_________________________________________________________________

	the user has changed the port and there is no client defined. tell the user what 
	happened, and make the error a non fatal one, so that s/he can see the info as far 
	as it exists. this is a warning, not an error, so an error dlg won't pop up in its
	annoying way.
	_________________________________________________________________
*/

BOOL CCmd_Info::HandledCmdSpecificError( LPCTSTR errBuf, LPCTSTR errMsg )
{
	if ( StrStr(errBuf, _T("Client unknown to server") ) ||
		 StrStr(errBuf, _T(" - use 'client' command to create it.") ) )
	{
        PostClientError();
	}

	return TRUE;
}

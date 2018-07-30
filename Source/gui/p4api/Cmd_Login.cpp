/*
 * Copyright 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// Cmd_Login.cpp

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_Login.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Login, CP4Command)


CCmd_Login::CCmd_Login(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4PASSWORD;
	m_TaskName= _T("Login");
}

BOOL CCmd_Login::Run(LPCTSTR password)
{
	m_Password = password;
	ClearArgs();
	AddArg(_T("login"));
	return CP4Command::Run();
}

void CCmd_Login::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	if(StrStr(data, _T("logged in")))
		TheApp()->StatusAdd( msg, SV_COMPLETION );
	else
	{
		m_ErrorTxt = msg;
		CP4Command::OnOutputInfo(level, data, msg);
	}
}

BOOL CCmd_Login::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
	BOOL handledError= FALSE;
	if(StrStr(errBuf, _T("Password invalid")) || StrStr(errBuf, _T("Password should be ")))
	{
		m_ErrorTxt= errMsg;
		TheApp()->StatusAdd( errMsg, SV_WARNING );
		handledError= TRUE;
	}

	return handledError;
}

#if 0
bool CCmd_Login::PrepPassword( CString &pwd )
{
    bool bOk = true;

	pwd.TrimLeft();
	pwd.TrimRight();

    // Don't allow non-ascii
    // for MBCS, we look at lead and trail bytes here, not characters,
    // but that's ok since the lead byte will be non-ascii and we'll be done
    for(int i = 0; bOk && i < pwd.GetLength(); i++)
        if(!_istascii(pwd.GetAt(i)))
		    bOk = false;

	if( bOk && pwd.Find(_T(' ')) != -1 )
		bOk = false;
    return bOk;
}
#endif

void CCmd_Login::OnPrompt( const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e )
{
	CString message = CharToCString(msg.Text());
	CString *csp;
	csp = &m_Password;
	rsp.Set(CharFromCString(*csp));
}

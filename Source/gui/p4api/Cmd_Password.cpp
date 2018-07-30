/*
 * Copyright 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// Cmd_Password.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_password.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Password, CP4Command)


CCmd_Password::CCmd_Password(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4PASSWORD;
	m_TaskName= _T("Password");
}

BOOL CCmd_Password::Run(LPCTSTR oldPwd, LPCTSTR newPwd)
{
	m_OldPwd = oldPwd;
	m_NewPwd = newPwd;
	ClearArgs();
	AddArg(_T("passwd"));
	return CP4Command::Run();
}

void CCmd_Password::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	if(StrStr(data, _T("Password updated")))
		TheApp()->StatusAdd( msg, SV_COMPLETION );
	else
	{
		m_ErrorTxt = StrCmp(data, _T("Password not changed.")) ? msg : data;
		CP4Command::OnOutputInfo(level, data, msg);
	}
}

BOOL CCmd_Password::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
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

bool CCmd_Password::PrepPassword( CString &pwd )
{
    bool bOk = true;

	pwd.TrimLeft();
	pwd.TrimRight();

    // Don't allow non-ascii
    // for MBCS, we look at lead and trail bytes here, not characters,
    // but that's ok since the lead byte will be non-ascii and we'll be done
    for(int i = 0; bOk && i < pwd.GetLength(); i++)
	{
        if(!_istascii(pwd.GetAt(i)))
		{
		    bOk = false;
			break;
		}
	}
    return bOk;
}

void CCmd_Password::OnPrompt( const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e )
{
	CString message = CharToCString(msg.Text());
	CString *csp;
	csp = (message.Find(_T("old")) != -1) ? &m_OldPwd : &m_NewPwd;
	rsp.Set(CharFromCString(*csp));
}

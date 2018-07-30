/*
 * Copyright 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// Cmd_Logout.cpp

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_Logout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Logout, CP4Command)


CCmd_Logout::CCmd_Logout(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4PASSWORD;
	m_TaskName= _T("Logout");
}

BOOL CCmd_Logout::Run(BOOL bInvalidating)
{
	ClearArgs();
	AddArg(_T("logout"));
	if (bInvalidating)
		AddArg(_T("-a"));
	return CP4Command::Run();
}

void CCmd_Logout::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	if(StrStr(data, _T("logged out")))
		TheApp()->StatusAdd( msg, SV_COMPLETION );
	else
	{
		m_ErrorTxt = msg;
		CP4Command::OnOutputInfo(level, data, msg);
	}
}

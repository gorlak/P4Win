//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Users.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_users.h"
#include "p4user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Users, CP4Command)


CCmd_Users::CCmd_Users(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4USERS;
	m_TaskName= _T("Users");
}

BOOL CCmd_Users::Run()
{
	ClearArgs();
	AddArg(_T("users"));

	return CP4Command::Run();
}

void CCmd_Users::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// Parse into a CP4Change and send that back
	CP4User *user= new CP4User;
	
    if( user->Create(data) )
	    m_List.AddTail(user);
    else
    {
		CString errMsg;
		errMsg.FormatMessage(IDS_INVALID_USER_s, msg);
		if (errMsg.Find(_T(" >")) != -1)
			errMsg += LoadStringResource(IDS_EMAILHASTRAILINGSPACE);
		TheApp()->StatusAdd(errMsg, SV_WARNING);
        delete user;
    }
}




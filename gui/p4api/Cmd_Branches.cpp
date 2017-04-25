//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Branches.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_branches.h"
#include "p4branch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Branches, CP4Command)


CCmd_Branches::CCmd_Branches(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4BRANCHES;
	m_TaskName= _T("Branches");
}

BOOL CCmd_Branches::Run()
{
	ClearArgs();
	AddArg(_T("branches"));

	m_UsedTagged = GET_SERVERLEVEL() >= 8 ? TRUE : FALSE;

	return CP4Command::Run();
}

void CCmd_Branches::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// Parse into a CP4Change and send that back
	CP4Branch *branch= new CP4Branch;
	if( branch->Create(data) )
        m_List.AddTail(branch);
    else
    {
        #ifdef _DEBUG
            CString errMsg;
            errMsg.Format(_T("Branch parse failed:\n%s"), data);
            TheApp()->StatusAdd(errMsg, SV_DEBUG);
        #endif
        delete branch;
    }
}

void CCmd_Branches::OnOutputStat( StrDict *varList )
{
	// Check for possible abort request
	if(APP_ABORTING())
	{
		ReleaseServerLock();
		ExitThread(0);
	}
	else
	{
        CP4Branch *client= new CP4Branch;
		if(client->Create(varList))
		    m_List.AddTail(client);
		else
			delete client;
	}
}


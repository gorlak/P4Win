//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Clients.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_clients.h"
#include "p4client.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Clients, CP4Command)


CCmd_Clients::CCmd_Clients(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4CLIENTS;
	m_TaskName= _T("Clients");
}

BOOL CCmd_Clients::Run()
{
	ClearArgs();
	AddArg(_T("clients"));

	m_UsedTagged = GET_SERVERLEVEL() >= 8 ? TRUE : FALSE;

	return CP4Command::Run();
}

void CCmd_Clients::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// Parse into a CP4Change and send that back
	CP4Client *client= new CP4Client;
	if( client->Create(data) )
	    m_List.AddTail(client);
    else
    {
        #ifdef _DEBUG
            CString errMsg;
            errMsg.Format(_T("Client parse failed:\n%s"), data);
            TheApp()->StatusAdd(errMsg, SV_DEBUG);
        #endif
        delete client;
    }
}

void CCmd_Clients::OnOutputStat( StrDict *varList )
{
	// Check for possible abort request
	if(APP_ABORTING())
	{
		ReleaseServerLock();
		ExitThread(0);
	}
	else
	{
        CP4Client *client= new CP4Client;
		if(client->Create(varList))
		    m_List.AddTail(client);
		else
			delete client;
	}
}





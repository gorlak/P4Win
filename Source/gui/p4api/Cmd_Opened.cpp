//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Opened.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_opened.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Opened, CP4Command)


CCmd_Opened::CCmd_Opened(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4OPENED;
	m_TaskName= _T("Opened");
}

BOOL CCmd_Opened::Run(BOOL allOpenFiles, BOOL bFilterByMyClient /*= FALSE*/,
					  int changeNumber /*= -1*/, CStringList *files /*=NULL*/)
{
    m_ChangeNumber= changeNumber;

	ClearArgs();
	m_BaseArgs=AddArg(_T("opened"));

	if(allOpenFiles)
	{
		m_BaseArgs=AddArg(_T("-a"));
		if (bFilterByMyClient && GET_P4REGPTR()->FilterPendChgsByMyClient() == 1)
		{
			CString filter = CString(_T("//")) + GET_P4REGPTR()->GetP4Client() + _T("/...");
			m_BaseArgs=AddArg(filter);
		}
	}

    // Add the changenumber arg, but only when running against a
    // 98.2 or newer server
	if(changeNumber >= 0 && GET_SERVERLEVEL() >= 4 )
	{
		AddArg(_T("-c"));

		if(changeNumber==0)
			AddArg(_T("default"));
		else
		{
			AddArg(changeNumber);
		}
	}

	if(files != NULL)
	{
		// Initilialize string list vars
		m_posStrListIn=files->GetHeadPosition();
		m_pStrListIn=files;  
	
		// Put the first few files into the arg list
		NextListArgs();
	}
	
	return CP4Command::Run();
}

void CCmd_Opened::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	CP4FileStats *stats=  new CP4FileStats;
	if(stats->Create(data))
    {
        // When running against a 97.3 server, implement the "-c" flag on the
        // client side, to filter for files open under a single change number
        if( GET_SERVERLEVEL() < 4 && m_ChangeNumber != -1 && stats->GetOpenChangeNum() != m_ChangeNumber )
            delete stats;
        else
		    m_List.AddHead(stats);
    }
	else
	{
		delete stats;
		CP4Command::OnOutputInfo(level, data, msg);
	}
}

BOOL CCmd_Opened::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
	if(StrStr(errBuf, _T("ile(s) not opened")))
		// Not really an error, so return 'Yes, we handled error'
		return TRUE;
	else
		return FALSE;
}

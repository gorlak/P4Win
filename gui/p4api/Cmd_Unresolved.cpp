//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Unresolved.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_unresolved.h"
#include "cmd_opened.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Unresolved, CP4Command)


CCmd_Unresolved::CCmd_Unresolved(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4UNRESOLVED;
	m_TaskName= _T("Unresolved");
}

BOOL CCmd_Unresolved::Run()
{
	// Set the base of arg list
	ClearArgs();

	if (GET_SERVERLEVEL() >= 18)	// use p4 fstat -Ru on 2004.2 or later servers
	{
		AddArg(_T("fstat"));
		AddArg(_T("-Ru"));
		CString str = CString(_T("//")) + GET_P4REGPTR()->GetP4Client() + _T("/...");
 		AddArg(str);
	}
	else							// use p4 resolve -n on servers before 2004.2
	{
		AddArg(_T("resolve"));
		AddArg(_T("-n"));
	}
			
	return CP4Command::Run();
}

// this fucntion is called only if p4 resolve -n (not fstat - Ru) was called
void CCmd_Unresolved::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
    CString sData(data);
	// Grab the client path, the first token on Resolve row.  Looks like:
	// d:\src\p4\Jamrules - merging //depot/main/p4/Jamrules#115,#116
	int end= sData.Find(_T("//"), 2);
	if (end != -1)
	{
		end = (sData.Left(end)).ReverseFind(_T('-'));
		if (end != -1)
		{
			if ((sData.GetAt(end - 1) == _T(' ')) && (sData.GetAt(end + 1) == _T(' ')))
				end--;
			else
				end = -1;
		}
	}
	if(end != -1)
		m_StrListOut.AddHead(sData.Left(end));
	else
		CP4Command::OnOutputInfo(level, data, msg);
}

// this fucntion is called only if p4 fstat -Ru (not resolve -n) was called
void CCmd_Unresolved::OnOutputStat( StrDict *varList )
{
	// Check for possible abort request
	if(APP_ABORTING())
	{
		ReleaseServerLock();
		ExitThread(0);
	}
	else
	{
        CP4FileStats *stats= new CP4FileStats;
		if(stats->Create(varList))
			m_UnresolvedArray.Add( stats );
		else
			delete stats;
	}
}

void CCmd_Unresolved::PostProcess()
{
	if( m_StrListOut.GetCount() > 0)	// m_StrListOut is empty for p4 fstat calls
	{
		CCmd_Opened cmd(m_pClient);
		cmd.Init(NULL, RUN_SYNC);

		if(cmd.Run(FALSE, FALSE, -1, &m_StrListOut))
		{
			m_FatalError= cmd.GetError();
			CObList const *wherelist= cmd.GetList();
			if(wherelist->GetCount() > 0)
			{	
				for(POSITION pos=wherelist->GetHeadPosition(); pos != NULL; )
				{
					CP4FileStats *stats= (CP4FileStats *) wherelist->GetNext(pos);
					m_UnresolvedArray.Add( stats );
				}
			}
		}
		else
		{
			m_ErrorTxt= _T("Unable to Run Opened");
			m_FatalError=TRUE;
		}
	}
}

BOOL CCmd_Unresolved::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
	if( StrStr(errBuf, _T(" - no such file(s)"))
	 ||	StrStr(errBuf, _T(" - file(s) not opened on this client")))
		return TRUE;
	if( StrStr(errBuf, _T("Operation 'user-resolve' fail")) )
	{
		CString txt;
		txt.FormatMessage( IDS_UNABLE_TO_RUN_RESOLVE_N_s, errMsg );  
		TheApp()->StatusAdd( txt, SV_ERROR );  
		return TRUE;
	}
	return StrStr(errBuf, _T("No file(s) to resolve")) != 0;
}

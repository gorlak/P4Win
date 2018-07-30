//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Fixes.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_fixes.h"
#include "p4fix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Fixes, CP4Command)


CCmd_Fixes::CCmd_Fixes(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4FIXES;
	m_TaskName= _T("Fixes");
}

BOOL CCmd_Fixes::Run( int changeNum/*=0*/, HTREEITEM change/*=NULL*/, 
					  BOOL bJob/*=FALSE*/, LPCTSTR jobname/*=NULL*/)
{
	m_ChangeItem= change;
    m_ChangeNumber= changeNum;

	ClearArgs();
	AddArg(_T("fixes"));
	if(changeNum > 0)
	{
		AddArg(_T("-c"));
        AddArg(changeNum);
	}
	else if (bJob)
	{
		AddArg(_T("-j"));
		AddArg(jobname);
	}
	
	return CP4Command::Run();
}

void CCmd_Fixes::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// Parse into a CP4Change and send that back
	CP4Fix *fix= new CP4Fix;
	if( fix->Create(data) )
	    m_List.AddHead(fix);
    else
    {
         #ifdef _DEBUG
            CString errMsg;
            errMsg.Format(_T("Fix parse failed:\n%s"), data);
            TheApp()->StatusAdd(errMsg, SV_DEBUG);
        #endif
        delete fix;
    }
}




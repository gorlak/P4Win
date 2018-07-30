//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Refersh.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_refresh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Refresh, CP4Command)


CCmd_Refresh::CCmd_Refresh(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4REFRESH;
	m_TaskName= _T("Refresh");
}

BOOL CCmd_Refresh::Run(CStringList *files)
{
	ASSERT(!files->IsEmpty());
	
	// Set the base of arg list
	ClearArgs();
	m_BaseArgs=AddArg(_T("refresh"));
	
	m_posStrListIn=files->GetHeadPosition();
	m_pStrListIn=files;  
	
	// Put the first few files into the arg list
	NextListArgs();
		
	return CP4Command::Run();
}

void CCmd_Refresh::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	if(StrStr(data, _T(" - refreshing")))
		TheApp()->StatusAdd(msg);
	else
		CP4Command::OnOutputInfo(level, data, msg);
}



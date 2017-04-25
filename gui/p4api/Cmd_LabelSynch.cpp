//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_LabelSynch.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_labelsynch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_LabelSynch, CP4Command)


CCmd_LabelSynch::CCmd_LabelSynch(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4LABELSYNC;
	m_TaskName= _T("LabelSynch");
	m_Preview = FALSE;
}

BOOL CCmd_LabelSynch::Run(LPCTSTR labelName, CStringList *files, 
					BOOL whatIf, BOOL removeFromLabel, BOOL noDeletes)
{
	ASSERT(labelName != NULL);
	ASSERT(!files->IsEmpty());
	
	// Set the base of arg list
	ClearArgs();
	
	AddArg(_T("labelsync"));
	if(removeFromLabel)
		AddArg(_T("-d"));
	if(noDeletes)
		AddArg(_T("-a"));
	if(whatIf)
	{
		m_Preview = TRUE;
		AddArg(_T("-n"));
	}
	AddArg(_T("-l"));
	m_BaseArgs =AddArg(labelName);
	
	m_posStrListIn=files->GetHeadPosition();
	m_pStrListIn=files;  

	// Put the first few files into the arg list
	NextListArgs();
		
	return CP4Command::Run();
}

void CCmd_LabelSynch::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	m_StrListOut.AddHead(data);
}




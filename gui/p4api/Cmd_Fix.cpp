//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Fix.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_fix.h"
#include "p4fix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Fix, CP4Command)


CCmd_Fix::CCmd_Fix(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4FIX;
	m_TaskName= _T("Fix");
}

BOOL CCmd_Fix::Run(CStringList *jobs, long changeNum, BOOL unfix/*=FALSE*/, LPCTSTR jobstatus/*=NULL*/)
{
	// Set the base of arg list
	ASSERT(!jobs->IsEmpty());
	if(changeNum == 0)
		{ ASSERT(0); return FALSE; }
	
	
	m_Unfixing= unfix;
	m_IsNewStatus= jobstatus ? TRUE : FALSE;

	// Set the base of arg list
	ClearArgs();
	AddArg(_T("fix"));
	if(jobstatus)
	{
		AddArg(_T("-s"));
		AddArg(jobstatus);
	}

	if(m_Unfixing && !m_IsNewStatus)
		AddArg(_T("-d"));

	AddArg(_T("-c"));
	m_BaseArgs=AddArg(changeNum);
	
	m_posStrListIn=jobs->GetHeadPosition();
	m_pStrListIn=jobs;  
	
	// Put the first few files into the arg list
	NextListArgs();
		
	return CP4Command::Run();
}

void CCmd_Fix::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	BOOL processedOutput= FALSE;
	CP4Fix *fix=  new CP4Fix;

	if(m_Unfixing && !m_IsNewStatus)
	{
		if(fix->CreateFromUnfix(data))
		{
			m_List.AddHead(fix);
			processedOutput= TRUE;
		}
		else
			delete fix;
	}
	else
	{
		if(fix->CreateMyFix(data))
		{
			m_List.AddHead(fix);
			processedOutput= TRUE;
		}
		else
			delete fix;
	}

	if(!processedOutput)
		CP4Command::OnOutputInfo(level, data, msg);
}

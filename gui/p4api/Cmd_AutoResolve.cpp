//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_AutoResolve.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_autoresolve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_AutoResolve, CP4Command)


CCmd_AutoResolve::CCmd_AutoResolve(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4AUTORESOLVE;
	m_TaskName= "AutoResolve";
}

BOOL CCmd_AutoResolve::Run(CStringList *files, int type, BOOL preview, BOOL force, 
						   BOOL textmerge, int whtSp)
{
	m_Preview= preview;

	// Set the base of arg list
	m_posStrListIn=files->GetHeadPosition();
	m_pStrListIn=files;  

	AddArg(_T("resolve"));
	switch (type)
	{
	case 0:
		m_BaseArgs=AddArg(_T("-at"));
		break;

	case 1:
		m_BaseArgs=AddArg(_T("-ay"));
		break;

	case 2:
		m_BaseArgs=AddArg(_T("-as"));
		break;

	case 3:
	default:
		m_BaseArgs=AddArg(_T("-am"));
		break;

	case 4:
		m_BaseArgs=AddArg(_T("-af"));
		break;
	}
	if(preview)
		m_BaseArgs=AddArg(_T("-n"));
	if(force)
		m_BaseArgs=AddArg(_T("-f"));
	if(textmerge)
		m_BaseArgs=AddArg(_T("-t"));

	if(whtSp && GET_SERVERLEVEL() >= 14)	// whitespace flag requires 2002.2 or later
	{
		switch (whtSp)
		{
		case 1:
			m_BaseArgs=AddArg(_T("-db"));
			break;
		case 2:
			m_BaseArgs=AddArg(_T("-dw"));
			break;
		case 3:
			m_BaseArgs=AddArg(_T("-dl"));
			break;
		case 0:
		default:
			break;
		}
	}
		
	// Put the first few files into the arg list
	if (!files->IsEmpty())
		NextListArgs();
		
	return CP4Command::Run();
}

void CCmd_AutoResolve::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	BOOL processedOutput=FALSE;
	CString temp;

	if(m_Preview)
	{
		temp = LoadStringResource(IDS_AUTOMERGE_PREVIEW) + msg;
	}
	else
		temp=msg;
		
	if(StrStr(data, _T(" - merging")))
	{
		TheApp()->StatusAdd(temp);
		if(m_Preview)
			m_StrListOut.AddHead(data);
		processedOutput=TRUE;
	}
	else if(StrStr(data, _T(" - binary/binary merge")))
	{
		TheApp()->StatusAdd(temp);
		if(m_Preview)
			m_StrListOut.AddHead(data);
		processedOutput=TRUE;
	}
	else if(StrStr(data, _T(" - vs")))
	{
		if (m_ReplyMsg == WM_THEIRFINDINDEPOT
		 || m_ReplyMsg == WM_THEIRHISTORY
		 || m_ReplyMsg == WM_THEIRPROPERTIES)
		{
			int i;
			if ((i = temp.Find( _T(" - vs "))) != -1)
			{
				temp = temp.Mid(i + sizeof(_T(" - vs "))/sizeof(TCHAR) -1 );
				temp.TrimLeft();
				temp = LoadStringResource(IDS_THEIR_FILE_IS) + temp;
			}
		}
		TheApp()->StatusAdd(temp);
		if(m_Preview)
			m_StrListOut.AddHead(data);
		processedOutput=TRUE;
	}
	else if(StrStr(data, _T(" + 0 conflicting")))
	{
		TheApp()->StatusAdd(temp);
		processedOutput=TRUE;
	}
	else if(IsValidMergeMessage(data))
	{
		TheApp()->StatusAdd(temp);
		m_StrListOut.AddHead(data);
		processedOutput=TRUE;
	}

	if(!processedOutput)
		CP4Command::OnOutputInfo(level, data, msg);
}


// Wrenched out of dmtypes.cc
static LPCTSTR DmtIntegHowFmt[] = { 
	_T("merge from"),
	_T("merge into"),
	_T("branch from"),
	_T("branch into"), 
	_T("copy from"),
	_T("copy into"),
	_T("ignored"),
	_T("ignored by"), 
	_T("delete from"),
	_T("delete into"),
	_T("edit from"),
	_T("add from"),
};

#define NUM_MERGETYPES (sizeof(DmtIntegHowFmt)/sizeof(LPCTSTR))

BOOL CCmd_AutoResolve::IsValidMergeMessage(CString const& data)
{
	for(int i=0; i< NUM_MERGETYPES; i++)
	{
		if(data.Find(DmtIntegHowFmt[i]) != -1)
			return TRUE;		
	}
	return FALSE;
}


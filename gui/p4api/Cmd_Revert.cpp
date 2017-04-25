//
// Copyright 2002 Perforce Software.  All rights reserved.
//
//

// Cmd_Revert.cpp

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_Revert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Revert, CP4Command)


CCmd_Revert::CCmd_Revert(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4REVERT;
	m_TaskName= _T("Revert");
	m_NbrNonEdits = m_NbrEdits = 0;
}

BOOL CCmd_Revert::Run( CString &Revertspec, BOOL bChgList /*=FALSE*/, 
					   BOOL bUnChgd /*=FALSE*/,  BOOL bPreview /*=FALSE*/, 
					   BOOL bVirtual /*=FALSE*/, BOOL bChkChgOnly /*=FALSE*/)
{
	m_TempList.AddHead(Revertspec);
	return Run(&m_TempList, bChgList, bUnChgd, bPreview, bVirtual);
}

BOOL CCmd_Revert::Run( CStringList *files, BOOL bChgList /*=FALSE*/, 
					   BOOL bUnChgd /*=FALSE*/,  BOOL bPreview /*=FALSE*/, 
					   BOOL bVirtual /*=FALSE*/, BOOL bChkChgOnly /*=FALSE*/)
{
	m_UnChgd = bUnChgd;
	m_Preview= bPreview;
	m_ChkChgOnly = bChkChgOnly;
	m_List.RemoveAll();
	ClearArgs();
	m_BaseArgs=AddArg(_T("revert"));
	if (bUnChgd)
	{
		if (bPreview && GET_SERVERLEVEL() < 14)		// 2002.2 or later?
			return FALSE;
		m_BaseArgs=AddArg(bPreview ? _T("-an") : _T("-a"));
	}
	if (bChgList)
		m_BaseArgs=AddArg(_T("-c"));
	if (bVirtual)
	{
		if (GET_SERVERLEVEL() >= 20)		// 2005.2 or later?
			m_BaseArgs=AddArg(_T("-v"));
		else
			return FALSE;
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

BOOL CCmd_Revert::NextListArgs()
{
	if (m_ChkChgOnly && m_NbrEdits)		// is this -an and we found an edit?
		return TRUE;					// return TRUE to indicate done = no mo args

	ClearArgs(m_BaseArgs);  // Clear all but the base args

    ASSERT(m_posStrListIn != NULL);
	
	// Pull another 20 files off the list
	int i;
	for(i=0; m_posStrListIn != NULL && i<20; i++)
		AddArg(m_pStrListIn->GetNext(m_posStrListIn));

	m_NbrEdits = i;

	// Caller knows not to call again when the list is empty
	if(m_posStrListIn == NULL)
		m_pStrListIn->RemoveAll();
	return FALSE;
}

void CCmd_Revert::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	if ( StrStr(data, _T(" - has pending integrations, not reverted")) )
		return;
    m_List.AddTail(data);
	m_NbrEdits--;
}

BOOL CCmd_Revert::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg )
{
	if ( StrStr(errBuf, _T("\nInvalid option: ")) )
	{
		return m_FatalError = TRUE; 
	}
	if ( StrStr(errBuf, _T(" - file(s) not opened for edit")) )
	{
		m_NbrNonEdits++;
		m_NbrEdits--;
		if (m_UnChgd && m_Preview)
			return TRUE;
	}
	return FALSE;
}

//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_PrepEdit.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_get.h"
#include "cmd_listopstat.h"
#include "cmd_prepedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_PrepEdit, CP4Command)


CCmd_PrepEdit::CCmd_PrepEdit(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4PREPEDIT;
	m_TaskName= _T("PrepEdit");
	m_WarnIfLocked = FALSE;
}


BOOL CCmd_PrepEdit::Run(LPCTSTR filespec, BOOL getHead, BOOL addFile)
{
	// Record the args
	m_FileName=filespec;
	m_GetHead=getHead;
	m_AddFile=addFile;
	return CP4Command::Run();
}

void CCmd_PrepEdit::PreProcess(BOOL& done)
{
	Error e;
	CStringList files;
	CString temp;

	// Get the file at head if required
	if(m_GetHead)
	{
		CCmd_Get *pCmd1= new CCmd_Get(m_pClient);
		pCmd1->Init(m_ReplyWnd, RUN_SYNC, HOLD_LOCK);
		pCmd1->SetOpeningForEdit(TRUE);
		temp= m_FileName+_T("#head");
		files.AddHead(temp);
		if(pCmd1->Run(&files, FALSE) )
			m_FatalError= pCmd1->GetError();
		else
		{
			delete pCmd1;
			m_ErrorTxt= _T("Unable to run Sync");
			m_FatalError=TRUE;
			return;
		}
        pCmd1->CloseConn(&e);
		// Make sure depot window gets updated (and pCmd1 is deleted)
		::PostMessage( m_ReplyWnd, pCmd1->GetReplyMsg(), (WPARAM) pCmd1, 0);
	}

	// Edit the file
	CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat(m_pClient);
	pCmd2->Init(m_ReplyWnd, RUN_SYNC);
	pCmd2->SetWarnIfLocked(m_WarnIfLocked);
	files.RemoveAll();
	files.AddHead(m_FileName);
	if(pCmd2->Run(&files, m_AddFile ? P4ADD : P4EDIT) )
		m_FatalError= pCmd2->GetError();
	else
	{
		delete pCmd2;
		m_ErrorTxt= _T("Unable to run Edit");
		m_FatalError=TRUE;
		return;
	}
    pCmd2->CloseConn(&e);
	// Make sure depot window gets updated (and pCmd1 is deleted)
	::PostMessage( m_ReplyWnd, pCmd2->GetReplyMsg(), (WPARAM) pCmd2, 0);

	// Note:
	// By setting done=TRUE, we prevent the main ExecCommand loop from running,
	// so this command will consist only of the subcommands that were invoked
	// in PreProcess()
	done=TRUE;
}




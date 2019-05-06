//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_ListOp.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_listop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_ListOp, CP4Command)


CCmd_ListOp::CCmd_ListOp(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4LISTOP;
	m_TaskName= _T("ListOp");
	m_OutputError = FALSE;
	m_ChkForSyncs = FALSE;
	m_WarnIfLocked= FALSE;
	m_StrListSync.RemoveAll();
}

BOOL CCmd_ListOp::Run(CStringList *files, int command, long changeNum, LPCTSTR newType)
{
	m_Command= command;
	
	m_posStrListIn=files->GetHeadPosition();
	m_pStrListIn=files;  
	
	ClearArgs();
	switch( m_Command )
	{
	case P4EDIT:
		m_BaseArgs=AddArg(_T("edit"));
		if(changeNum > 0)
		{
			AddArg(_T("-c"));
			m_BaseArgs=AddArg(changeNum);
		}
		break;
	case P4DELETE:
		m_BaseArgs=AddArg(_T("delete"));
		if(changeNum > 0)
		{
			AddArg(_T("-c"));
			m_BaseArgs=AddArg(changeNum);
		}
		break;
	case P4LOCK:
		ASSERT(changeNum==0);
		m_BaseArgs=AddArg(_T("lock"));
		break;
	case P4UNLOCK:
		ASSERT(changeNum==0);
		m_BaseArgs=AddArg(_T("unlock"));
		break;
	case P4REVERT:
		ASSERT(changeNum==0);
		m_BaseArgs=AddArg(_T("revert"));
		break;
	case P4VIRTREVERT:
		ASSERT(changeNum==0);
		m_BaseArgs=AddArg(_T("revert"));
		m_BaseArgs=AddArg(_T("-v"));
		break;
	case P4REVERTUNCHG:
		ASSERT(changeNum==0);
		m_BaseArgs=AddArg(_T("revert"));
		m_BaseArgs=AddArg(_T("-a"));
		break;
	case P4REOPEN:
		m_BaseArgs=AddArg(_T("reopen"));
		if(changeNum > 0)
		{
			AddArg(_T("-c"));
			m_BaseArgs=AddArg(changeNum);
		}
		else if(changeNum==0)
		{
			AddArg(_T("-c"));
			m_BaseArgs=AddArg(_T("default"));
		}
		else if(newType != NULL)
		{
			AddArg(_T("-t"));
			m_BaseArgs=AddArg(newType);
		}
		else
			ASSERT(0);
		break;
	case P4ADD:
		m_BaseArgs=AddArg(_T("add"));
		break;
	default:
		ASSERT(0);
	}
	
	m_posStrListIn=files->GetHeadPosition();
	m_pStrListIn=files;  
	
	// Put the first few files into the arg list
	NextListArgs();
	
	return CP4Command::Run();
}

void CCmd_ListOp::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	BOOL processedMessage=FALSE;

	switch(m_Command)
	{
	case P4EDIT:
		if(StrStr(data, _T("opened for edit")))
		{
			m_StrListOut.AddHead(data);
			processedMessage=TRUE;
		}
		else if(m_ChkForSyncs)
        {
            LPCTSTR p = StrStr(data, _T(" - must sync/resolve "));
            if(p)
    			m_StrListSync.AddHead(CString(data, int(p - data)));
		}
		break;
		
	case P4REOPEN:
		if(StrStr(data, _T(" - reopened;")))
		{
			m_StrListOut.AddHead(data);
			processedMessage=TRUE;
		}
		break;

	case P4REVERT:
	case P4VIRTREVERT:
	case P4REVERTUNCHG:
		if(!StrStr(data, _T("not opened on")))
		{
			m_StrListOut.AddHead(data);
			TheApp()->StatusAdd(msg);
			processedMessage=TRUE;
		}
		break;

	case P4LOCK:
		if(StrStr(data, _T("- locking")))
		{
			m_StrListOut.AddHead(data);
			processedMessage=TRUE;
		}
		break;

	case P4UNLOCK:	
		if(StrStr(data, _T("- unlocking")))
		{
			m_StrListOut.AddHead(data);
			processedMessage=TRUE;
		}
		break;

	case P4DELETE:
		if(StrStr(data, _T("opened for delete")))
		{
			m_StrListOut.AddHead(data);
			processedMessage=TRUE;
		}
		break;
	case P4ADD:
		if(StrStr(data, _T("opened for add")))
		{
			m_StrListOut.AddHead(data);
			processedMessage=TRUE;
		}
		break;
	default:
		ASSERT(0);
	}

	if(!processedMessage)
	{
		TheApp()->StatusAdd(msg, SV_WARNING);
		if(m_WarnIfLocked && StrStr(data, _T(" - locked by ")))
			AfxMessageBox(msg);
	}
}


BOOL CCmd_ListOp::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
    BOOL handledError=FALSE;
    
    const TCHAR chmod[] = _T("chmod: ");
    const int chmodLen = (sizeof(chmod)-sizeof(TCHAR))/sizeof(TCHAR);

    LPCTSTR end = StrStr(errBuf, _T(": The system cannot find the file specified") );

    if( !StrNCmp(errBuf, chmod, chmodLen) && end)
    {
        // Format a message for display
        CString msg;
        msg.FormatMessage( IDS_UNABLE_TO_FIND_FILE_s_ON_CLIENT,
                    CString(errBuf+chmodLen, int(end-errBuf)-chmodLen) );
        TheApp()->StatusAdd( msg, SV_WARNING );

        handledError=TRUE;
    }

	if (!handledError)
		m_OutputError = TRUE;
    return handledError;
}

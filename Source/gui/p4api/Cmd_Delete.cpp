//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Changes.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_delete.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Delete, CP4Command)


CCmd_Delete::CCmd_Delete(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4DELETE;
	m_TaskName= _T("Delete");
	m_IgnoreActiveItem = FALSE;
}

BOOL CCmd_Delete::Run(int descType, LPCTSTR reference)
{
	ClearArgs();
	switch (descType)
	{
	case P4BRANCH_DEL:
		AddArg(_T("branch"));	break;
	case P4CHANGE_DEL:
		AddArg(_T("change"));	break;
	case P4CLIENT_DEL:
		AddArg(_T("client"));	break;
	case P4JOB_DEL:
		AddArg(_T("job"));		break;
	case P4LABEL_DEL:
		AddArg(_T("label"));	break;
	case P4USER_DEL:
		AddArg(_T("user"));		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	
	AddArg(_T("-d"));
//how come no -f for force?

	// remove any surrounding quotes (jobs can have multi word names)
	CString	ref = reference;
	ref.TrimLeft(_T('\"'));
	ref.TrimRight(_T('\"'));
	AddArg(ref);

	m_CompletionData=_T("");
	m_CompletionMsg =_T("");
	return CP4Command::Run();
}

void CCmd_Delete::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	if(StrStr(data, _T("can't be deleted")))
	{
		if(StrStr(data, _T("User")) && StrStr(data, _T("open on")))
			TheApp()->StatusAdd( LoadStringResource(IDS_USER_HAS_OPEN_FILES_CANT_DELETE), SV_ERROR);
		else
			TheApp()->StatusAdd(msg, SV_WARNING);
		m_FatalError=TRUE;
	}
	else if(StrStr(data, _T("Can't delete branch")))
	{
		if(StrStr(data, _T("owned by")))
			TheApp()->StatusAdd( LoadStringResource(IDS_CANT_DELETE_UNOWNED_BRANCH), SV_ERROR);
		else
			TheApp()->StatusAdd(msg, SV_WARNING);
		m_FatalError=TRUE;
	}
	else
	{
		m_CompletionData= data;
		m_CompletionMsg = msg;
	}
}


/*
	_________________________________________________________________
*/

BOOL CCmd_Delete::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
	m_FatalError = TRUE;

    if( StrStr(errBuf, _T("use -f to force delete")))
	{
		CString m;
        m.FormatMessage(IDS_CANT_DELETE_PRIVELEGE_s, CString(errBuf, (int)(StrStr(errBuf, _T("; use -f")) - errBuf )));
		TheApp()->StatusAdd( m, SV_ERROR);
		return m_FatalError;
	}

	if(StrStr(errBuf, _T("Can't delete client")) && StrStr(errBuf, _T("owned by")))
	{
		TheApp()->StatusAdd(LoadStringResource(IDS_CANT_DELETE_UNOWNED_CLIENT), SV_ERROR);
		return m_FatalError;
	}

	if(StrStr(errBuf, _T("Can't delete label")))
	{
		if(StrStr(errBuf, _T("owned by")))
			TheApp()->StatusAdd( LoadStringResource(IDS_CANT_DELETE_UNOWNED_LABEL), SV_ERROR);
		else
			TheApp()->StatusAdd(errMsg, SV_ERROR );
		return m_FatalError;
	}

	if(StrStr(errBuf, _T("Can't delete branch")))
	{
		if(StrStr(errBuf, _T("owned by")))
			TheApp()->StatusAdd( LoadStringResource(IDS_CANT_DELETE_UNOWNED_BRANCH), SV_ERROR);
		else
			TheApp()->StatusAdd(errMsg, SV_ERROR);
		return m_FatalError;
	}

	if (( StrStr(errBuf, _T("don't have permission"))) 
	 || ( StrStr(errBuf, _T("is locked and can't be deleted"))))

	{
		TheApp()->StatusAdd( errMsg, SV_ERROR );
		return m_FatalError;
	}

	m_FatalError = FALSE;
	return m_FatalError;
}

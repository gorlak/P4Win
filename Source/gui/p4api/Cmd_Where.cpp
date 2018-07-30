//
// Copyright 2001 Perforce Software.  All rights reserved.
//
//

// Cmd_Where.cpp

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_Where.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Where, CP4Command)


CCmd_Where::CCmd_Where(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4WHERE;
	m_TaskName= _T("Where");
}

BOOL CCmd_Where::Run( CString &filespec )
{
	ClearArgs();
	AddArg(_T("where"));
	AddArg(filespec);
	return CP4Command::Run();
}

BOOL CCmd_Where::Run( CStringList *filelist )
{
    ASSERT_KINDOF( CStringList, filelist );
	ASSERT( filelist->GetCount() );

	ClearArgs();
	m_BaseArgs= AddArg(_T("where"));

    m_posStrListIn= filelist->GetHeadPosition();
	m_pStrListIn= filelist;  
	
	// Put the first few files into the arg list
	NextListArgs();

	return CP4Command::Run();
}

void CCmd_Where::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	int i;
    CString str = data;
	if ((i = str.Find(_T(" //"))) != -1)
	{
		m_Depot.AddTail(str.Left(i));
		str = str.Mid(i+1);
		if ((i = str.Find(_T(":\\"))) != -1)
		{
			i -= 2;
			m_Client.AddTail(str.Left(i));
			m_Local.AddTail(str.Mid(i+1));
		}
		else if (((i = str.Find(_T(":/"))) != -1) && (str.GetAt(i-2) <= _T(' ')))
		{
			i -= 2;
			m_Client.AddTail(str.Left(i));
			CString local = str.Mid(i+1);
			local.Replace(_T('/'), _T('\\'));
			m_Local.AddTail(local);
		}
		else
		{
			m_Client.AddTail(_T("{Unknown}"));
			m_Local.AddTail(_T("{Unknown}"));
		}
	}
	else m_Depot.AddTail(_T("{Unknown}"));
}

BOOL CCmd_Where::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg )
{
	if ( StrStr(errBuf, _T(" - file(s) not in client view")) )
	{
		return TRUE ; 
	}
	return ( FALSE );
}

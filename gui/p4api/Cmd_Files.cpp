//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Files.cpp

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_Files.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Files, CP4Command)


CCmd_Files::CCmd_Files(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4FILES;
	m_TaskName= _T("Files");
}

BOOL CCmd_Files::Run( CString &filespec )
{
	m_List.RemoveAll();
	ClearArgs();
	AddArg(_T("files"));
	AddArg(filespec);
	return CP4Command::Run();
}

void CCmd_Files::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
    m_List.AddHead(data);
}

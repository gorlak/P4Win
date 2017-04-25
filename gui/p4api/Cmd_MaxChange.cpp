//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Changes.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_maxchange.h"
#include "cmd_info.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_MaxChange, CP4Command)


CCmd_MaxChange::CCmd_MaxChange(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4MAXCHANGE;
	m_TaskName= _T("MaxChange");
	if(GET_SERVERLEVEL()==0)
		m_StartedNoServerLevel=TRUE;
	else
		m_StartedNoServerLevel=FALSE;
}

BOOL CCmd_MaxChange::Run( )
{
	ClearArgs();
	AddArg(_T("changes"));
	AddArg(_T("-s"));
	AddArg(_T("submitted"));
	AddArg(_T("-m"));
	AddArg(_T("1"));
	
	m_MaxChange=0;
	return CP4Command::Run();
}

void CCmd_MaxChange::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// A 'P4 Changes' row - just get the change number 
    LPCTSTR p = StrStr(data, _T("Change"));
    if(p)
	{
        // find the change number
        static TCHAR digits[] = _T("0123456789");

        int posDigits = StrCSpn(data, digits);
        if(posDigits || _istdigit(data[0]))
		{
            m_MaxChange = _ttoi(data+posDigits);
		}
	}
	else
		CP4Command::OnOutputInfo(level, data, msg);
}




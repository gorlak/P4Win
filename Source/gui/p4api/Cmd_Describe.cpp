//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Changes.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_describe.h"
#include "p4specdata.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Describe, CP4Command)



CCmd_Describe::CCmd_Describe(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4DESCRIBE;
	m_TaskName= _T("Describe");
	m_Flag = 0;
	m_bLong = FALSE;
	m_CallingListCtrl = NULL;
}

BOOL CCmd_Describe::Run(int descType, LPCTSTR reference, 
						LPCTSTR templateName/*=NULL*/, 
						BOOL force/*=FALSE*/,
						int flag  /*= 0*/, 
						BOOL uFlag/*=FALSE*/)
{
	ClearArgs();
	m_SpecType=descType;
	switch (m_SpecType)
	{
	case P4DESCRIBELONG:
		m_bLong = TRUE;
		m_SpecType=P4DESCRIBE;
		AddArg(_T("describe"));
		switch (m_Flag = flag)
		{
		case ID_SHOWDIFFS_SUMMARY:
			AddArg(_T("-ds"));
			break;
		case ID_SHOWDIFFS_UNIFIED:
			AddArg(_T("-du"));
			break;
		case ID_SHOWDIFFS_CONTEXT:
			AddArg(_T("-dc"));
			break;
		case ID_SHOWDIFFS_RCS:
			AddArg(_T("-dn"));
			break;
		case ID_SHOWDIFFS_NONE:
			m_Flag = 0;
			AddArg(_T("-s"));
			break;
		}
		break;
	case P4DESCRIBE:
		AddArg(_T("describe"));
		AddArg(_T("-s"));
		break;
	case P4BRANCH_SPEC:
		AddArg(_T("branch"));	
		AddArg(_T("-o"));
		break;
	case P4CHANGE_SPEC:
		AddArg(_T("change"));	
		AddArg(_T("-o"));
		if( uFlag && GET_SERVERLEVEL() >= 23 )	// 2007.2 or later?
			AddArg(_T("-u"));
		else if( force || uFlag )
			AddArg(_T("-f"));
		break;
	case P4CLIENT_SPEC:
        AddArg(_T("client"));	
		AddArg(_T("-o"));
        if( templateName!=NULL )
        {
            AddArg(_T("-t"));
            AddArg(templateName);
        }
        break;
	case P4JOB_SPEC:
		AddArg(_T("job"));		
		AddArg(_T("-o"));
		break;
	case P4LABEL_SPEC:
		AddArg(_T("label"));	
		AddArg(_T("-o"));
		if( templateName!=NULL )
        {
            AddArg(_T("-t"));
            AddArg(templateName);
        }
		break;
	case P4USER_SPEC:
		AddArg(_T("user"));		
		AddArg(_T("-o"));
		break;
	case P4GROUP_SPEC:
		AddArg(_T("group"));		
		AddArg(_T("-o"));
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	
	if(reference != NULL && _tcslen(reference) > 0)
	{
		m_Reference = reference;
		m_Reference.TrimLeft(_T('\"'));
		m_Reference.TrimRight(_T('\"'));
		AddArg(m_Reference);
	}
	else m_Reference = _T("");
	m_Description=_T("");

	return CP4Command::Run();
}

void CCmd_Describe::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	m_Description += data;
    m_Description += g_CRLF;
}

void CCmd_Describe::OnOutputText(LPCTSTR data, int length)
{
	CString txt(data, length);
	m_Description += txt + g_CRLF;
}

void CCmd_Describe::OnOutputStat( StrDict *varList )
{
}

void CCmd_Describe::PostProcess()
{
	//		Lose the trailing crlf 
	//
	int len=m_Description.GetLength();
	if(len > 1)
	{
		ASSERT(m_Description.Mid(m_Description.GetLength()-2).Compare( g_CRLF ) == 0);
		
		LPTSTR buf=m_Description.GetBuffer(len);
		buf[len-2]=_T('\0');
		m_Description.ReleaseBuffer(len-2);
	}

	//		if the spec string is empty we have a pre 98.3 server.
	//		so use our hard coded strings with the new format
	//		and with the old data.
	//
	if(m_SpecStr.IsEmpty())
	{
		switch(m_SpecType)
		{
		case P4DESCRIBE:
		case P4CHANGE_SPEC:
			m_SpecStr = CHANGE_SPEC;
			break;
		case P4BRANCH_SPEC:
			m_SpecStr = BRANCH_SPEC;
			break;
		case P4CLIENT_SPEC:
			m_SpecStr = CLIENT_SPEC;
			break;
		case P4JOB_SPEC:
			m_SpecStr = JOB_SPEC;
			break;
		case P4LABEL_SPEC:
			m_SpecStr = LABEL_SPEC;
			break;
		case P4USER_SPEC:
			m_SpecStr = USER_SPEC;
			break;
		case P4GROUP_SPEC:
			m_SpecStr = GROUP_SPEC;
			break;
		default:
			ASSERT(0);
		}
	}
}


#define NOTRANS _T("No Translation for parameter 'data' value '")

BOOL CCmd_Describe::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
	BOOL handledError=TRUE;
#ifndef UNICODE
	if (!StrNCmp(errBuf, NOTRANS, sizeof(NOTRANS)-1))
	{
		CString utf8;
		TCHAR *p = (TCHAR *)errBuf + sizeof(NOTRANS)-1;
		while (*p && *p != '\'')
		{
			TCHAR c1 = *p++;
			TCHAR c2 = *p++;
			if (_istdigit(c1))
				c1 &= 0x0F;
			else
				c1 = (c1 & 0x0F) + 9;
			if (_istdigit(c2))
				c2 &= 0x0F;
			else
				c2 = (c2 & 0x0F) + 9;
			TCHAR c3 = (c1 << 4) + c2;
			utf8.Insert(0x7FFFFFFF, c3);
		}

		// allocate widechar buffer and convert into it
		int lenw = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, 0, 0);
		LPWSTR utf16 = (LPWSTR)::VirtualAlloc(NULL, lenw*sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, utf16, lenw);

		// allocate char buffer and convert back into it, using '?' for
		// unmappable characters
		int len = WideCharToMultiByte(CP_ACP, 0, utf16, -1, 0, 0, NULL, NULL);
		LPSTR buf = (LPSTR)::VirtualAlloc(NULL, len*sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
		WideCharToMultiByte(CP_ACP, 0, utf16, -1, buf, len, NULL, NULL);

		// append to description
		m_Description += buf;
		m_Description += g_CRLF;

		::VirtualFree(buf, 0, MEM_RELEASE);
		::VirtualFree(utf16, 0, MEM_RELEASE);
		handledError = FALSE;
	}
	else
#endif
	{
		m_FatalError = TRUE;
		TheApp()->StatusAdd( errMsg, SV_WARNING, true );	
	}
	return handledError;
}

//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Get.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_get.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Get, CP4Command)


CCmd_Get::CCmd_Get(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4GET;
	m_TaskName= _T("Sync");
	m_OutputRows= m_AddCount= 0;
	m_bIntegAfterSync = FALSE;
	m_bOpenAfterSync = FALSE;
	m_bOpeningForEdit = FALSE;
	m_RevHistWnd = 0;
	m_RevReq = 0;
}

BOOL CCmd_Get::Run(CStringList *files, BOOL whatIf, BOOL bRefresh )
{
	ASSERT(!files || !files->IsEmpty());

	m_Warnings.RemoveAll();
	m_OutputRows= 0;

	m_WhatIf= whatIf;

	// Set the base of arg list
	ClearArgs();
	m_BaseArgs=AddArg(_T("sync"));
	
	if(whatIf)
		m_BaseArgs=AddArg(_T("-n"));
	if ( bRefresh )
		m_BaseArgs = AddArg( _T("-f") );
	
	if (files)
	{
		m_posStrListIn=files->GetHeadPosition();
		m_pStrListIn=files;  
		
		// Put the first few files into the arg list
		NextListArgs();
	}
		
	return CP4Command::Run();
}

void CCmd_Get::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	CString prefix;

	m_OutputRows++;

	if(m_WhatIf)
		prefix = LoadStringResource(IDS_SYNC_PREVIEW);
	
	
    // When getting an open file at an earlier rev, the message is as follows:
    // I'm still not certain why you would want to do the get, but it IS
    // possible.  The server resonse looks like one of the following:
    // //xxxx/a/cow/spion/swine/q.txt#1 - is opened at a later revision - not changed
    // //xxxx/a/cow/spion/swine/q.txt#2 - is opened
    if(StrStr(data,_T(" - is opened")) &&
       !StrStr(data,_T(" - is opened for edit and can't be ")) &&
       !StrStr(data,_T(" - is opened for add and can't be ")) &&
       !StrStr(data,_T(" - is opened for delete and can't be ")) &&
       !StrStr(data,_T(" - is opened at a later revision - not changed")) )
   	{
		TheApp()->StatusAdd(prefix+msg);
		m_GetList.AddHead(data);
	}
	else if(StrStr(data, _T(" - updating")))
	{
		TheApp()->StatusAdd(prefix+msg);
		m_GetList.AddHead(data);
	}
	else if(StrStr(data, _T(" - added as")))
	{
		TheApp()->StatusAdd(prefix+msg);
		m_GetList.AddHead(data);
		m_AddCount++;
	}
    else if(StrStr(data, _T(" - refreshing")))
    {
		TheApp()->StatusAdd(msg);
        m_GetList.AddHead(data);
    }
	else if(StrStr(data, _T(" - deleted as")))
	{
		TheApp()->StatusAdd(prefix+msg);
		m_RemoveList.AddHead(data);
	}
	else
	{
		// Must be some sort of warning, so add it to the warning 
		// summary
		m_Warnings.Add( msg );
		CP4Command::OnOutputInfo(level, data, msg);
	}
}


void CCmd_Get::OnOutputError(char level, LPCTSTR errBuf, LPCTSTR errMsg)
{
	CString txt(errBuf);
	CString msg(errMsg);

	if (( txt.Find( _T("Perforce password") ) > -1 ) 
	 || ( txt.Find( _T("please login") ) > -1 ))
	{
		SET_PWD_ERROR(TRUE);
        TheApp()->StatusAdd( msg, SV_WARNING );
		m_ErrorTxt= LoadStringResource(IDS_OPERATION_CANNOT_COMPLETED_BECAUSE_BAD_PASSWORD);
        m_FatalError=TRUE;
		return;
	}

	m_OutputRows++;

	if(txt.Find(/*F or f*/_T("ile(s) up-to-date"))>=0)
		TheApp()->StatusAdd(msg);  // Not really an error
	else if ((IsRunIntegAfterSync()) && (txt.Find(_T("no such file"))>=0))
		;										// Not really an error if sync for integ
	else if(txt.Find(_T("Can't clobber writeable file")) >=0 ||
	   txt.Find(_T("can't create directory for")) >=0 ||
	   txt.Find(_T("filename, directory name, or volume label syntax is incorrect")) >=0 )
	{
		// Must be some sort of warning, so add it to the warning 
		// summary
		m_Warnings.Add( msg );
		TheApp()->StatusAdd(msg, SV_WARNING);  // Not really an error
		RemoveLastFromGetList();
	}
	else
	{
		// Must be some sort of warning, so add it to the warning 
		// summary
		m_Warnings.Add( msg );
		TheApp()->StatusAdd(msg, SV_WARNING);
	}
}

int CCmd_Get::ReiterateWarnings()
{
	INT_PTR i;

	if( (i = m_Warnings.GetSize()) > 0 )
	{
		TheApp()->StatusAdd(_T("SYNC OPERATION WARNING SUMMARY:"), SV_WARNSUMMARY);

		for( int i=0; i < m_Warnings.GetSize(); i++ )
		{
			TheApp()->StatusAdd( m_Warnings.GetAt( i ), SV_WARNING );
		}
	
		CString summary;
		summary.Format(_T("Total of %d warnings reported"), m_Warnings.GetSize());
		TheApp()->StatusAdd( summary, SV_WARNSUMMARY );
	}
	else
	{
		TheApp()->StatusAdd(_T("Sync completed with no warnings or errors"), SV_COMPLETION);
	}

	return int(i);
}


// A wacky kludge:
// If a get fails due to noclobber flag, the server will first 
// indicate a successfull get, and then recant this later when it
// sees the error, so we need to a way to remove the last item 
// from the get list
void CCmd_Get::RemoveLastFromGetList()
{
	ASSERT(!m_GetList.IsEmpty());

	if(!m_GetList.IsEmpty())
		m_GetList.RemoveHead();
}

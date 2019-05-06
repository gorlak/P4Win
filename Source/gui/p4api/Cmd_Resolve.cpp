//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Resolve.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_resolve.h"
#include "merge\GuiClientMerge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Resolve, CP4Command)


CCmd_Resolve::CCmd_Resolve(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4RESOLVE;
	m_TaskName= _T("Resolve Interactive");
	m_bHeadIsText = FALSE;	// TRUE means we know for sure, so init to FALSE
	m_BaseMergeFileName = _T("");
}

BOOL CCmd_Resolve::Run(LPCTSTR filespec, BOOL bForce, BOOL bTextualMerge)
{
	m_Resolved=FALSE;

	ClearArgs();
	AddArg(_T("resolve"));
	if (GET_SERVERLEVEL() >= 18)	// 2004.1 server or later?
		AddArg(_T("-o"));
	if (bForce)
		AddArg(_T("-f"));
	if (bTextualMerge)
		AddArg(_T("-t"));
	AddArg(m_filespec = filespec);

	return CP4Command::Run();
}

//////////////////////////////////////////////////////
// Butt-ugly handling of text messages for resolve operations
// Hopefully a tagged version of resolve will eliminate the need
//
void CCmd_Resolve::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	CString temp;
	CString orgmsg = msg;

    TCHAR *pBase = StrStr(data, _T(" using base "));
	if(pBase)
	{
		m_BaseMergeFileName= pBase + StrLen(_T(" using base "));
		*pBase = _T('\0');
	}
    LPCTSTR pMerging = StrStr(data, _T(" - merging"));
    LPCTSTR pVs;
	if(pMerging)
	{
		TheApp()->StatusAdd(orgmsg);
		m_YourMergeFileName= CString(data, int(pMerging - data));
		m_TheirMergeFileName= pMerging + StrLen(_T(" - merging"));
	}
	else if( HandleBinaryMergeMessage(data) )
		TheApp()->StatusAdd(orgmsg);
	else if((pVs = StrStr(data, _T(" - vs"))) != 0)
	{
		TheApp()->StatusAdd(orgmsg);
		m_YourMergeFileName= CString(data, int(pVs - data));
		m_TheirMergeFileName= pVs + StrLen(_T(" - vs"));
	}
	else if(IsValidMergeMessage(data))
	{
		TheApp()->StatusAdd(orgmsg);
		m_Resolved = TRUE;
	}
	else
		CP4Command::OnOutputInfo(level, data, orgmsg);
}

int CCmd_Resolve::OnResolve(ClientMerge *m, Error *e)
{
    CGuiClientMerge merge(m);
	merge.SetFilespec(m_filespec);
	merge.SetHeadIsText(m_bHeadIsText);
    merge.SetBaseFileName(m_BaseMergeFileName);
    merge.SetYourFileName(m_YourMergeFileName);
    merge.SetTheirFileName(m_TheirMergeFileName);

    if(m->GetBaseFile())
	    ::PostMessage(GetReplyWnd(), WM_P4MERGE3, (WPARAM) &merge, GetServerKey());
    else
    	::PostMessage(GetReplyWnd(), WM_P4MERGE2, (WPARAM) &merge, GetServerKey());
    merge.WaitForSignal();
    return merge.GetStatus();
}

BOOL CCmd_Resolve::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
    BOOL handledError=FALSE;
    
    if( StrStr(errBuf, _T("No file(s) to resolve") ))
    {
        // The gui does not consider this an error, so quell the output
        handledError=TRUE;
    }
	else if( StrStr(errBuf, _T("Cannot create a file when that file already exists") ))
	{
		// Give a more meaningful error dialog
		CString txt;
		txt.FormatMessage(IDS_CANTRENAMEISITLOCKED_s, m_YourMergeFileName);
		AfxMessageBox(txt, MB_ICONSTOP);
	}

    return handledError;
}


// Wrenched out of dmtypes.cc
static LPCTSTR DmtIntegHowFmt[] = 
{ 
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

BOOL CCmd_Resolve::IsValidMergeMessage( CString const& data)
{
	for(int i=0; i< NUM_MERGETYPES; i++)
	{
		if(data.Find(DmtIntegHowFmt[i]) != -1)
			return TRUE;		
	}
	return FALSE;
}


// Binary merge messages:
// The messages are formatted in DmResolveData::UserMessage( )
// If we find a first-half message immediately followed by
// a second half message, it is 'safe' to assume we have extracted
// the delimiter between their file and your file

static LPCTSTR FileTypeFirstHalf[] = 
{ 
	_T(" - text/"),
	_T(" - ctext/"),
	_T(" - cxtext/"),
	_T(" - ltext/"),
	_T(" - ktext/"),
	_T(" - ttext/"),
	_T(" - xtext/"),
	_T(" - xltext/"),
	_T(" - kxtext/"),
	_T(" - binary/"),
	_T(" - xbinary/"),
	_T(" - ubinary/"),
	_T(" - tempobj/"),
	_T(" - xtempobj/")
	_T(" - unicode/"),
	_T(" - xunicode/")
};

static LPCTSTR FileTypeSecondHalf[] = 
{ 
	_T("text merge "),
	_T("ctext merge "),
	_T("cxtext merge "),
	_T("ltext merge "),
	_T("ktext merge "),
	_T("ttext merge "),
	_T("xtext merge "),
	_T("xltext merge "),
	_T("kxtext merge "),
	_T("binary merge "),
	_T("xbinary merge "),
	_T("ubinary merge "),
	_T("tempobj merge "),
	_T("xtempobj merge ")
	_T("unicode merge "),
	_T("xunicode merge ")
};

#define NUM_MERGEFILETYPES (sizeof(FileTypeFirstHalf)/sizeof(LPCTSTR))

BOOL CCmd_Resolve::HandleBinaryMergeMessage( CString const& data )
{
    ASSERT(sizeof(FileTypeFirstHalf) == sizeof(FileTypeSecondHalf));

	int offset, len;

	for( int i=0; i < NUM_MERGEFILETYPES; i++ )
	{
		if( (offset= data.Find(FileTypeFirstHalf[i])) != -1 )
		{
			len= lstrlen(FileTypeFirstHalf[i]);
			CString txt2= data.Mid( offset + len );
			
			for( int j=0; j < NUM_MERGEFILETYPES; j++ )
			{
				if( txt2.Find( FileTypeSecondHalf[j]) == 0 )
				{
					len += lstrlen( FileTypeSecondHalf[j] );
					m_YourMergeFileName= data.Left( offset );
					m_TheirMergeFileName= data.Mid( offset + len );
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}



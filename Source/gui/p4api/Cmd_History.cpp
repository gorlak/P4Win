//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_History.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_history.h"
#include "cmd_fstat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_History, CP4Command)


CCmd_History::CCmd_History(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4HISTORY;
	m_TaskName= _T("History");
	m_pFileStats= NULL;
	m_InitialRev= -1;
	m_OverrideDashI = FALSE;
	m_OverrideDashM = FALSE;
	m_DashIoverride = 0;
	m_DashMoverride = 1;
	m_EnableShowIntegs = TRUE;
	m_IsAFile = TRUE;
	m_WriteToTempFile = FALSE;
	m_pOutputFile = NULL;
	m_KeyToHold = 0;
}

CCmd_History::~CCmd_History()
{
	if(m_pOutputFile != NULL)
		delete m_pOutputFile;
}

BOOL CCmd_History::Run(LPCTSTR fileSpec)
{
	int	i;

	TheApp()->m_RevHistLast = 0;

	ASSERT(fileSpec != NULL);
	m_FileName= fileSpec;
	m_FileNbr = 0;
		
	// Set the arg list
	ClearArgs();
	AddArg(_T("filelog"));
	AddArg((GET_SERVERLEVEL() >= 19
		 && GET_P4REGPTR()->GetUseShortRevHistDesc()) ? _T("-L") : _T("-l"));
	if( GET_SERVERLEVEL() >= 8 )
	{
		if (m_OverrideDashI)
		{
			if (GET_P4REGPTR()->GetEnableRevHistShowIntegs( ))
			{
				if (m_DashIoverride == 1)
				{
					AddArg(_T("-i"));
				}
				else if (m_DashIoverride == 2)
				{
					AddArg(GET_SERVERLEVEL() >= 24 ? _T("-h") : _T("-i"));	// 2007.3 or later?
				}
			}
		}
		else if ( (i=GET_P4REGPTR()->GetFetchCompleteHist()) > 0 )
		{
			if (GET_P4REGPTR()->GetEnableRevHistShowIntegs( ))
				AddArg((i==1 || GET_SERVERLEVEL() < 24) ? _T("-i") : _T("-h"));
			else
				GET_P4REGPTR()->SetFetchCompleteHist( 0 );
		}
		if (m_OverrideDashM)
		{
			if (m_DashMoverride)
			{
				AddArg(_T("-m"));
				AddArg(TheApp()->m_RevHistLast = m_DashMoverride);
			}
		}
		else if ( !GET_P4REGPTR()->GetFetchAllHist() )
		{
			int count= GET_P4REGPTR()->GetFetchHistCount();
			if( count > 0 )
			{
				AddArg(_T("-m"));
				AddArg(TheApp()->m_RevHistLast = count + TheApp()->m_RevHistMore);
			}
		}

		// if the server is new enough, 
		// get the time as well as the date
		if (GET_SERVERLEVEL() >= 14)
			AddArg(_T("-t"));
	}
	AddArg(fileSpec);

	m_TextOut.Empty();
	m_BranchInfo.Empty();

	m_History.Clear();
	TheApp()->m_RevHistMore = 0;

	return CP4Command::Run();
}


void CCmd_History::PreProcess(BOOL &done)
{
	// if we are to just write this to a temp file,
	// create the temp file and we are done.
	if (m_WriteToTempFile)
	{
		m_pOutputFile= FileSys::Create( FST_TEXT );
		CString TempPath= GET_P4REGPTR()->GetTempDir();
		CString FileName = m_FileName;
		int i;
		if (((i = FileName.ReverseFind(_T('\\'))) != -1)
		 || ((i = FileName.ReverseFind(_T('/'))) != -1))
			FileName = FileName.Mid(i+1);
		Error e;

		for(i=0; i< 100; i++)
		{
			e.Clear();
			m_TempName.Format(_T("%s\\FileLog-Rev-%d-%s.txt"), TempPath, i, FileName);
			m_pOutputFile->Set(CharFromCString(m_TempName));
			
			if( !e.Test() )
				m_pOutputFile->MkDir( &e );// Prepare write (makes dir as required)
			if( !e.Test() )
			{
				m_pOutputFile->Perms( FPM_RW );			// Open it
				m_pOutputFile->Open( FOM_WRITE, &e );
			}
			if(!e.Test())
				break;
		}
		if(e.Test())
		{
			m_ErrorTxt.Format(_T("Error opening temporary file:\n %s"), m_TempName);
			TheApp()->StatusAdd(m_ErrorTxt, SV_ERROR);
			m_FatalError=TRUE;

			delete m_pOutputFile;
			m_pOutputFile=NULL;
			done=TRUE;
		}
		return;
	}			// end of write to temp file code; normal operation follows
	
	// Run fstat synchronously
	CCmd_Fstat cmd(m_pClient);
	cmd.Init( NULL, RUN_SYNC );
	if( cmd.Run( TRUE, m_FileName, TRUE ) )
	{
		CObList *list= cmd.GetFileList();
		if( list->GetCount() > 0 )
		{
			m_pFileStats= (CP4FileStats *) list->GetHead();
			if ( list->GetCount() > 1 )
			{
				POSITION pos = list->GetHeadPosition();
				list->GetNext(pos);
				while (pos)
				{
					CP4FileStats *fs = (CP4FileStats *) list->GetNext(pos);
					delete fs;
				}
			}
			return;
		}
	}
	
	m_ErrorTxt= cmd.GetErrorText();
	m_FatalError= TRUE;
	done=TRUE;
}


void CCmd_History::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// if we are to just write this to a temp file,
	// write the data to a new line and we are done.
	if (m_WriteToTempFile)
	{
		CString str = data;
		StrBuf sptr;
		Error e;
		if (str[0] == _T('/'))
			str += _T('\n');
		else if (str[0] == _T('#'))
			str = _T("... ") + str;
		else if (str[0] > _T(' '))
			str = _T("... ... ") + str + _T("\n\n");
		sptr.Set(const_cast<char*>((const char*)CharFromCString(str)));
		m_pOutputFile->Write( &sptr, &e );
		if(e.Test())
			m_FatalError= TRUE;
		return;
	}

	// Server spews filename, followed by changedescription,
	// followed optionally by branchinfo
	switch(level)
	{
	case '0': 
		if( !m_TextOut.IsEmpty() )
			m_History.AddRevision(m_FileName, m_FileNbr, m_TextOut, m_BranchInfo);
		m_TextOut.Empty();
		m_BranchInfo.Empty();
		m_FileName= data;
		m_FileNbr++;
		break;
	case '1':
		if( !m_TextOut.IsEmpty() )
			m_History.AddRevision(m_FileName, m_FileNbr, m_TextOut, m_BranchInfo);
		m_TextOut= data;
		m_BranchInfo.Empty();
		break;
	case '2':
		if( !m_BranchInfo.IsEmpty() )
			m_BranchInfo += _T("; ");
		m_BranchInfo += data;
		break;
	default:
		ASSERT(0);
	}
}

void CCmd_History::PostProcess()
{
	// if we are to just write this to a temp file,
	// close the file and we are done.
	if (m_WriteToTempFile)
	{
		Error e;
		m_pOutputFile->ClearDeleteOnClose();
		m_pOutputFile->Close(&e);
	}

	// Catch any straggler data 
	if( !m_TextOut.IsEmpty() )
		m_History.AddRevision(m_FileName, m_FileNbr, m_TextOut, m_BranchInfo);
}

BOOL CCmd_History::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
	BOOL handledError= FALSE;
	if(StrStr(errBuf, _T("Remote depot 'db.rev' database access failed")))
	{
		m_ErrorTxt= errMsg;
		TheApp()->StatusAdd( errMsg, SV_WARNING );
		m_FatalError = handledError = TRUE;
	}

	return handledError;
}

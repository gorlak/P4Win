//
// 
// Copyright 1997,1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

// Cmd_Fstat.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_fstat.h"
#include "tokenstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//		don't confuse these. different case
//
const CString g_sClientViewOnly = _T("-C");
const CString g_sChangeList   = _T("-c");
const CString g_sChangeList_e = _T("-e");
const CString g_sMaxOutput = _T("-m");

IMPLEMENT_DYNCREATE(CCmd_Fstat, CP4Command)


CCmd_Fstat::CCmd_Fstat(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4FSTAT;
	m_TaskName= _T("Fstat");
	m_pBatch= NULL;
	m_IncludeAddedFiles = m_bWorking = FALSE;
}


CCmd_Fstat::~CCmd_Fstat()
{
	if(m_pBatch != NULL)
		delete m_pBatch;
}

BOOL CCmd_Fstat::Run( BOOL suppress, LPCTSTR spec, BOOL bShowEntireDepot, long minChange)
{
    m_SpecList.RemoveAll();
    m_SpecList.AddHead( spec );
    return Run( suppress, &m_SpecList, bShowEntireDepot, minChange );
}

BOOL CCmd_Fstat::Run(BOOL suppress, CStringList *specList, BOOL bShowEntireDepot, 
					 long minChange/*=0*/, BOOL bWorking/*=FALSE*/, 
					 long wkChgNbr/*=-1*/, long maxOutput/*=0*/)
{
    ASSERT_KINDOF( CStringList, specList );
	ASSERT( specList->GetCount() );
	
	ClearArgs();
	m_BaseArgs= AddArg(_T("fstat"));

	if(GET_SERVERLEVEL() >= 19)			// 2005.1 or later?
	{
		m_BaseArgs= AddArg(_T("-Ol"));
		m_BaseArgs= AddArg(_T("-Oh"));
		if (bWorking)
		{
			m_bWorking = TRUE;
			m_BaseArgs= AddArg(_T("-Ro"));
			if (wkChgNbr != -1)
			{
				AddArg( g_sChangeList_e );
				if (!wkChgNbr)
					m_BaseArgs= AddArg(_T("default"));
				else
					m_BaseArgs= AddArg(wkChgNbr);
			}
		}
	}
	else if (bWorking)
	{
		ASSERT(0);
		return FALSE;
	}

	if(suppress)
		m_BaseArgs= AddArg(_T("-s"));
	if(minChange > 0)
	{
		AddArg( g_sChangeList );
		m_BaseArgs= AddArg(minChange);
	}

	if ( ! bShowEntireDepot && GET_SERVERLEVEL() > 3 )
		m_BaseArgs= AddArg ( g_sClientViewOnly );

	if ( maxOutput && GET_SERVERLEVEL() > 20 )
	{
		if (maxOutput == -1)
			m_IgnorePermissionErrs = TRUE;
		else
		{
			m_BaseArgs= AddArg ( g_sMaxOutput );
			m_BaseArgs= AddArg ( maxOutput );
			if (maxOutput == 1)
				m_IgnorePermissionErrs = TRUE;
		}
	}

	m_pBatch= new CObList;

    m_posStrListIn= specList->GetHeadPosition();
	m_pStrListIn= specList;  
	
	// Put the first few files into the arg list
	NextListArgs();

	return CP4Command::Run();
}

BOOL CCmd_Fstat::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
    if ((StrStr(errBuf, _T(" - file(s) not in client view")))
	 ||	(StrStr(errBuf, _T(" - protected namespace - access denied")))
	 || (m_bWorking && StrStr(errBuf, _T(" - file(s) not opened on this client"))))
	{
        return TRUE;
	}
    else if ((GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		  && (StrStr(errBuf, _T(" - no mappings in client view"))))
	{
		m_ErrorList.AddHead(errBuf);
        return TRUE;
	}
	else if ( StrStr(errBuf, _T("no such file") )  )
	{
		TheApp()->StatusAdd( LoadStringResource(IDS_NO_FILES_UNDER_FOLDER), SV_DEBUG );  
		return TRUE ; 
	}
	else if ( StrStr(errBuf, _T(" database access failed.") )  )
	{
		m_FatalError = TRUE;
		return FALSE;
	}

	return StrStr(errBuf, _T("up-to-date.") ) != 0;
}

void CCmd_Fstat::OnOutputStat( StrDict *varList )
{
	// Check for possible abort request
	if(APP_ABORTING())
	{
		ReleaseServerLock();
		ExitThread(0);
	}
	else
	{
        CP4FileStats *stats= new CP4FileStats;
		if(stats->Create(varList))
			AddFstatRow(stats);
		else
			delete stats;
	}
}

void CCmd_Fstat::AddFstatRow(CP4FileStats *stats)
{
	// Files open by other users for add or new integ arent reported by fstat, but files open 
	// for add or new branch by this client are, so suppress them - they arent in the depot yet
	if( !m_IncludeAddedFiles && 
		(stats->GetMyOpenAction() == F_ADD || stats->GetMyOpenAction() == F_BRANCH) && 
		stats->GetHeadRev() == 0  )
	{
		delete stats;	
		return;
	}

	// Note: GUI should work this list from head to tail to preserve order
	m_pBatch->AddTail(stats);
	if(!m_IsChildTask && m_pBatch->GetCount() > 49)
	{
		// Send a full batch to gui
		if ( m_ReplyWnd )
		{
            // First get a this ptr and the list into a suitable wrapper
            CFstatWrapper *pWrap= new CFstatWrapper;
            pWrap->pCmd= this;
            pWrap->pList= m_pBatch;

			::PostMessage(m_ReplyWnd, m_ReplyMsg, (WPARAM) pWrap, -1 );
			m_pBatch= new CObList;
		}
	}
}


void CCmd_Fstat::PostProcess()
{
	if(!m_IsChildTask && m_pBatch->GetCount() > 0)
	{
		// Send a partial batch to gui (and only if there's a window to receive it)
		if ( m_ReplyWnd )
		{
            // First get a this ptr and the list into a suitable wrapper
            CFstatWrapper *pWrap= new CFstatWrapper;
            pWrap->pCmd= this;
            pWrap->pList= m_pBatch;

			::PostMessage(m_ReplyWnd, m_ReplyMsg, (WPARAM) pWrap, -1 );
			m_pBatch= NULL;
		}
	}
}


/*
	_________________________________________________________________

	use this when running fstat synchronously, since we cant post 
	messages, etc.
	_________________________________________________________________
*/

CObList *CCmd_Fstat::GetFileList( )
{
	return m_pBatch;
}
